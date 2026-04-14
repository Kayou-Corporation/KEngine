#include "Public/Shader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

BEGIN_NAMESPACE_RHI

std::string GetShaderName(const std::string& path)
{
    std::filesystem::path p(path);
    return p.stem().string();
}

std::string HashFile(const std::string& content, const std::string& entry)
{
    std::hash<std::string> strHash;
    size_t hash = strHash(content + entry);

    return std::to_string(hash);
}

void ShaderCompiler::Initialize()
{
    createGlobalSession(m_globalSession.writeRef());

#if defined(_WIN32)
    constexpr uint8_t nbTargets = 2;
    slang::TargetDesc targets[nbTargets] = {};

    targets[0].format = SLANG_SPIRV;
    targets[0].profile = m_globalSession->findProfile("spirv_1_5");

    targets[1].format = SLANG_DXIL;
    targets[1].profile = m_globalSession->findProfile("sm_6_6");
#else
    constexpr uint8_t nbTargets = 1;
    slang::TargetDesc targets[nbTargets] = {};

    targets[0].format = SLANG_SPIRV;
    targets[0].profile = m_globalSession->findProfile("spirv_1_5");
#endif

    slang::SessionDesc desc = {};
    desc.targets = targets;
    desc.targetCount = nbTargets;

    m_globalSession->createSession(desc, m_session.writeRef());
}

ShaderData ShaderCompiler::Load(const std::string& file, const ShaderStage& stage) const
{
    ShaderData bin{};

	const std::string fullFile = "Engine/Assets/Shaders/" + file + ".slang";

    std::ifstream fileContent(fullFile, std::ios::binary);
    if (!CheckIsFileOpenOrValid(fileContent, fullFile))
        return bin;

    std::string content((std::istreambuf_iterator<char>(fileContent)), std::istreambuf_iterator<char>());

    std::string name = GetShaderName(fullFile);

    const std::string entry = ShaderStageToEntry(stage);
    const std::string hash = HashFile(content, entry);

    const std::string reflectionPath = CacheShaderPath(hash, name, ".kayou");
    const std::string spirvPath = CacheShaderPath(hash, name, ".spv");

    bool pathExists = std::filesystem::exists(spirvPath) && std::filesystem::exists(reflectionPath);

#if defined(_WIN32)
    const std::string dxilPath = CacheShaderPath(hash, name, ".dxil");
    
    pathExists = pathExists && std::filesystem::exists(dxilPath);
#endif

    if (pathExists)
    {
        std::ifstream r(reflectionPath, std::ios::binary);
        if (CheckIsFileOpenOrValid(r, reflectionPath))
            bin.descriptors = ReadDescriptors(r);

        std::ifstream s(spirvPath, std::ios::binary);
        if (CheckIsFileOpenOrValid(s, spirvPath))
            bin.spirv.assign(std::istreambuf_iterator<char>(s), std::istreambuf_iterator<char>());

#if defined(_WIN32)
        std::ifstream d(dxilPath, std::ios::binary);
        if (CheckIsFileOpenOrValid(d, dxilPath))
            bin.dxil.assign(std::istreambuf_iterator<char>(d), std::istreambuf_iterator<char>());
#endif

        return bin;
    }

    bin = Compile(fullFile, content, entry, stage);

    const std::string shaderCacheDir = "Cache/Shaders";

    std::filesystem::create_directories(shaderCacheDir);

    for (auto& f : std::filesystem::directory_iterator(shaderCacheDir))
    {
        if (f.path().filename().string().starts_with(name))
            std::filesystem::remove(f.path());
    }

    std::ofstream r(reflectionPath, std::ios::binary);
    if (CheckIsFileOpenOrValid(r, reflectionPath))
        WriteDescriptors(r, bin.descriptors);

    std::ofstream s(spirvPath, std::ios::binary);
    if (CheckIsFileOpenOrValid(s, spirvPath))
        s.write(reinterpret_cast<char*>(bin.spirv.data()), bin.spirv.size());

#if defined(_WIN32)
    std::ofstream d(dxilPath, std::ios::binary);
    if (CheckIsFileOpenOrValid(d, dxilPath))
        d.write(reinterpret_cast<char*>(bin.dxil.data()), bin.dxil.size());
#endif

    return bin;
}

ShaderData ShaderCompiler::Compile(const std::string& file, const std::string& content, const std::string& entry, const ShaderStage& stage) const
{
    ShaderData bin{};

    Slang::ComPtr<slang::IModule> slangModule;
    Slang::ComPtr<ISlangBlob> diagnostics;

    slangModule = m_session->loadModuleFromSourceString(file.c_str(), file.c_str(), content.c_str(), diagnostics.writeRef());

    if (diagnostics)
    {
        spdlog::error("Slang diagnostics for {}:\n{}", file, static_cast<const char*>(diagnostics->getBufferPointer()));
    }

    if (!slangModule)
    {
        spdlog::error("Failed to load module for shader {}", file);
        return {};
    }

    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    SlangResult result = slangModule->findEntryPointByName(entry.c_str(), entryPoint.writeRef());

    if (SLANG_FAILED(result) || !entryPoint)
    {
        spdlog::error("Wrong entry point for shader {}: {}", file, entry);
        return {};
    }

    slang::IComponentType* components[] =
    {
        slangModule,
        entryPoint
    };

    Slang::ComPtr<slang::IComponentType> composedProgram;

    result = m_session->createCompositeComponentType(components, 2, composedProgram.writeRef(), diagnostics.writeRef());

    if (diagnostics)
    {
        spdlog::error("Slang compose diagnostics for {}:\n{}", file, static_cast<const char*>(diagnostics->getBufferPointer()));
    }

    if (SLANG_FAILED(result) || !composedProgram)
    {
        spdlog::error("Failed to compose program for shader {}", file);
        return {};
    }

    Slang::ComPtr<slang::IComponentType> linkedProgram;

    result = composedProgram->link(linkedProgram.writeRef(), diagnostics.writeRef());

    if (diagnostics)
    {
        spdlog::error("Slang link diagnostics for {}:\n{}", file, static_cast<const char*>(diagnostics->getBufferPointer()));
    }

    if (SLANG_FAILED(result) || !linkedProgram)
    {
        spdlog::error("Failed to link shader {}", file);
        return {};
    }

#if defined(_WIN32)
    constexpr int nbTargets = 2;
#else
    constexpr int nbTargets = 1;
#endif

    for (int target = 0; target < nbTargets; target++)
    {
        Slang::ComPtr<ISlangBlob> code;

        result = linkedProgram->getEntryPointCode(0, target, code.writeRef(), diagnostics.writeRef());

        if (diagnostics)
        {
            spdlog::error("Slang codegen diagnostics for {}:\n{}", file,  static_cast<const char*>(diagnostics->getBufferPointer()));
        }

        if (SLANG_FAILED(result) || !code)
        {
            spdlog::error("Failed to generate shader code for target {} in {}", target, file);
            continue;
        }

        uint8_t* data = (uint8_t*)code->getBufferPointer();
        size_t size = code->getBufferSize();

        if (!data || size == 0)
        {
            spdlog::error("Generated shader code is empty for {}", file);
            continue;
        }

#if defined(_WIN32)
        if (target == 0)
            bin.spirv.assign(data, data + size);
        else
            bin.dxil.assign(data, data + size);
#else
        bin.spirv.assign(data, data + size);
#endif
    }

    if (bin.spirv.empty())
    {
        spdlog::warn("SPIR-V compilation failed for {}", file);
    }

#if defined(_WIN32)
    if (bin.dxil.empty())
    {
        spdlog::warn("DXIL compilation failed for {}", file);
    }
#endif

	bin.descriptors = Reflect(linkedProgram->getLayout(), stage);

    return bin;
}

std::vector<Descriptor> ShaderCompiler::Reflect(slang::ProgramLayout* layout, const ShaderStage& stage)
{
    const auto globals = layout->getGlobalParamsTypeLayout();

    const uint32_t count = globals->getFieldCount();
    const uint32_t descCount = static_cast<uint32_t>(globals->getBindingRangeCount());

    std::vector<Descriptor> descriptors;
    descriptors.resize(descCount);

    for (uint32_t i = 0; i < count; ++i)
    {
        slang::VariableLayoutReflection* field = globals->getFieldByIndex(i);

        uint32_t set = field->getBindingSpace();
        const uint32_t bindingIndex = field->getBindingIndex();
        slang::TypeLayoutReflection* typeLayout = field->getTypeLayout();

        uint32_t descriptorCount = 1;

        if (typeLayout->getKind() == slang::TypeReflection::Kind::Array)
        {
            descriptorCount = static_cast<uint32_t>(typeLayout->getElementCount());
        }

        descriptors[i].index = set;

        Binding binding;
        binding.index = bindingIndex;
        binding.type = typeLayout->getBindingRangeType(0);
        binding.shape = typeLayout->getType()->getResourceShape();
		binding.count = descriptorCount;
        binding.stage = stage;
        descriptors[i].bindings.push_back(binding);
    }

	return descriptors;
}

void ShaderCompiler::WriteDescriptors(std::ofstream& out, const std::vector<Descriptor>& descriptors) const
{
	uint32_t descriptorCount = static_cast<uint32_t>(descriptors.size());
	Core::Write(out, descriptorCount);

    for (const auto& descriptor : descriptors)
    {
        Core::Write(out, descriptor.index);
        uint32_t bindingCount = static_cast<uint32_t>(descriptor.bindings.size());
        Core::Write(out, bindingCount);
		for (const auto& binding : descriptor.bindings)
		{
			Core::Write(out, binding.index);
			Core::Write(out, binding.stage);
			Core::Write(out, binding.count);
			Core::Write(out, binding.type);
			Core::Write(out, binding.shape);
		}
    }
}

std::vector<Descriptor> ShaderCompiler::ReadDescriptors(std::ifstream& in) const
{
	std::vector<Descriptor> descriptors;
	uint32_t descriptorCount;

	Core::Read(in, descriptorCount);

	descriptors.resize(descriptorCount);
	for (uint32_t i = 0; i < descriptorCount; ++i)
	{
		Core::Read(in, descriptors[i].index);
		uint32_t bindingCount;
		Core::Read(in, bindingCount);
		descriptors[i].bindings.resize(bindingCount);
		for (uint32_t j = 0; j < bindingCount; ++j)
		{
			Core::Read(in, descriptors[i].bindings[j].index);
			Core::Read(in, descriptors[i].bindings[j].stage);
			Core::Read(in, descriptors[i].bindings[j].count);
			Core::Read(in, descriptors[i].bindings[j].type);
			Core::Read(in, descriptors[i].bindings[j].shape);
		}
	}
	return descriptors;
}

END_NAMESPACE_RHI
