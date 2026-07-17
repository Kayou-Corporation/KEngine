#include "RHI/API/Shader.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <spdlog/spdlog.h>

BEGIN_NAMESPACE_CORE

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

ShaderData ShaderCompiler::Load(const std::string& file, const ShaderStage& stage, bool isGlobalLayout, bool usesGlobalLayout) const
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
            ReadReflectionData(r, bin, isGlobalLayout, usesGlobalLayout);

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

    bin = Compile(fullFile, content, entry, stage, isGlobalLayout, usesGlobalLayout);

    const std::string shaderCacheDir = "Cache/Shaders";

    std::filesystem::create_directories(shaderCacheDir);

    for (auto& f : std::filesystem::directory_iterator(shaderCacheDir))
    {
        if (f.path().filename().string().starts_with(name))
            std::filesystem::remove(f.path());
    }

    std::ofstream r(reflectionPath, std::ios::binary);
    if (CheckIsFileOpenOrValid(r, reflectionPath))
        WriteReflectionData(r, bin, isGlobalLayout, usesGlobalLayout);

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

ShaderData ShaderCompiler::Compile(const std::string& file, const std::string& content, const std::string& entry, const ShaderStage& stage, bool isGlobalLayout, bool usesGlobalLayout) const
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

	Reflect(bin, linkedProgram->getLayout(), stage, isGlobalLayout, usesGlobalLayout);

    return bin;
}

ShaderData ShaderCompiler::Reflect(ShaderData& bin, slang::ProgramLayout* layout, const ShaderStage& stage, bool isGlobalLayout, bool usesGlobalLayout)
{
    // Reflect vertex input layout if this is a vertex shader
    if (stage == ShaderStage::Vertex)
    {
        bin.vertexAttributes = ReflectVertexInputs(layout);

        if (!bin.vertexAttributes.empty())
        {
            VertexBindingLayout binding{};
            binding.binding = 0;
            binding.stride = 0;
            binding.inputRate = VertexInputRate::PerVertex;

            for (const Kayou::Core::VertexAttributeLayout& attribute : bin.vertexAttributes)
            {
                binding.stride += GetFormatSize(attribute.format);
            }

            bin.vertexBindings.push_back(binding);
        }
    }

    if (!isGlobalLayout && usesGlobalLayout)
        return bin;

    slang::TypeLayoutReflection* const globals = layout->getGlobalParamsTypeLayout();
    const uint32_t count = globals->getFieldCount();
    uint32_t trueSetCount = 0;

    std::map<uint32_t, Descriptor> descriptorMap;
    std::map<uint32_t, PushConstant> pushConstantMap;
    uint32_t pushConstantIdx = 0;

    for (uint32_t i = 0; i < count; ++i)
    {
        slang::VariableLayoutReflection* field = globals->getFieldByIndex(i);
        if (!field)
            continue;
    
        slang::TypeLayoutReflection* typeLayout = field->getTypeLayout();
    
        if (typeLayout->getKind() == slang::TypeReflection::Kind::ParameterBlock)
        {
            trueSetCount++;

            uint32_t setIndex = field->getBindingIndex();

            std::string setName = field->getName();

            Descriptor& descriptorSet = descriptorMap[setIndex];
            descriptorSet.index = setIndex;
            descriptorSet.name = setName;

            slang::TypeLayoutReflection* elementTypeLayout = typeLayout->getElementTypeLayout();
            const uint32_t bindingCount = elementTypeLayout->getFieldCount();

            descriptorSet.bindings.reserve(bindingCount);
            for (uint32_t j = 0; j < bindingCount; ++j)
            {
                slang::VariableLayoutReflection* subField = elementTypeLayout->getFieldByIndex(j);
                if (!subField)
                    continue;

                slang::TypeLayoutReflection* subTypeLayout = subField->getTypeLayout();

                const uint32_t bindingIndex = subField->getBindingIndex();

                const slang::BindingType bindingType = subTypeLayout->getBindingRangeType(0);

                const SlangResourceShape bindingShape = subTypeLayout->getType()->getResourceShape();

                uint32_t descriptorCount = 1;
                if (typeLayout->getKind() == slang::TypeReflection::Kind::Array)
                {
                    descriptorCount = static_cast<uint32_t>(typeLayout->getElementCount());
                }

                std::string bindingName = subField->getName();

                Binding binding{};
                binding.index = bindingIndex;
                binding.type = bindingType;
                binding.shape = bindingShape;
                binding.count = descriptorCount;
                binding.stage = isGlobalLayout ? ShaderStage::All : stage;
                binding.name = bindingName;

                descriptorSet.bindings.push_back(binding);
            }
        }
        else if (typeLayout->getBindingRangeCount() > 0 && typeLayout->getBindingRangeType(0) == slang::BindingType::PushConstant)
        {
            std::string name = field->getName();

            slang::TypeLayoutReflection* elementTypeLayout = typeLayout->getElementTypeLayout();
            if (!elementTypeLayout)
                elementTypeLayout = typeLayout;

            uint32_t size = static_cast<uint32_t>(elementTypeLayout->getSize());
            uint32_t offset = static_cast<uint32_t>(field->getOffset());

            PushConstant& pushConstant = pushConstantMap[pushConstantIdx];
            pushConstant.name = name;
            pushConstant.size = size;
            pushConstant.offset = offset;
            pushConstant.stage = isGlobalLayout ? ShaderStage::All : stage;

            pushConstantIdx++;

            const uint32_t subFieldCount = elementTypeLayout->getFieldCount();
            for (uint32_t j = 0; j < subFieldCount; ++j)
            {
                slang::VariableLayoutReflection* subField = elementTypeLayout->getFieldByIndex(j);
                if (!subField) 
                    continue;

                std::string subfieldName = subField->getName();

                uint32_t subfieldSize = static_cast<uint32_t>(subField->getTypeLayout()->getSize(SLANG_PARAMETER_CATEGORY_UNIFORM));

                uint32_t subfieldOffset = static_cast<uint32_t>(subField->getOffset(SLANG_PARAMETER_CATEGORY_UNIFORM));

                PushConstant& subfieldPushConstant = pushConstantMap[pushConstantIdx];
                subfieldPushConstant.name = subfieldName;
                subfieldPushConstant.size = subfieldSize;
                subfieldPushConstant.offset = subfieldOffset;
                subfieldPushConstant.stage = isGlobalLayout ? ShaderStage::All : stage;

                pushConstantIdx++;
            }
        }
    }

    std::vector<Descriptor> descriptors;
    descriptors.reserve(trueSetCount);
    for (auto& [fst, snd] : descriptorMap)
    {
        descriptors.push_back(snd);
    }

    std::vector<PushConstant> pushConstants;
    pushConstants.reserve(pushConstantIdx + 1);
    for (auto& [fst, snd] : pushConstantMap)
    {
        pushConstants.push_back(snd);
    }

    bin.descriptors = std::move(descriptors);
    bin.pushConstants = std::move(pushConstants);

    return bin;
}

std::vector<VertexAttributeLayout> ShaderCompiler::ReflectVertexInputs(slang::ProgramLayout* layout)
{
    std::vector<VertexAttributeLayout> attributes;

    if (!layout)
        return attributes;

    slang::EntryPointReflection* entryPointLayout = layout->getEntryPointByIndex(0);
    if (!entryPointLayout)
        return attributes;

    uint32_t paramCount = entryPointLayout->getParameterCount();
    if (paramCount == 0)
        return attributes;

    slang::VariableLayoutReflection* param = entryPointLayout->getParameterByIndex(0);
    if (!param)
        return attributes;

    slang::TypeLayoutReflection* paramTypeLayout = param->getTypeLayout();
    if (!paramTypeLayout)
        return attributes;

    uint32_t fieldCount = paramTypeLayout->getFieldCount();
    uint32_t currentOffset = 0;

    for (uint32_t i = 0; i < fieldCount; ++i)
    {
        slang::VariableLayoutReflection* field = paramTypeLayout->getFieldByIndex(i);
        if (!field)
            continue;

        slang::TypeLayoutReflection* fieldTypeLayout = field->getTypeLayout();
        if (!fieldTypeLayout)
            continue;

        //const char* semantic = field->getSemanticName();
        uint32_t location = i;

        VertexAttributeLayout attr;
        attr.location = location;
        attr.binding = 0;
        attr.offset = currentOffset;
        attr.format = GetFormatFromSlangType(fieldTypeLayout);
        attr.name = field->getName();

        attributes.push_back(attr);

        currentOffset += GetFormatSize(attr.format);
    }

    return attributes;
}

void ShaderCompiler::WriteReflectionData(std::ofstream& out, const ShaderData& bin, bool isGlobalLayout, bool usesGlobalLayout) const
{
    WriteVertexAttributes(out, bin.vertexAttributes);
    WriteVertexBindings(out, bin.vertexBindings);

    if (!isGlobalLayout && usesGlobalLayout)
        return;

    WriteDescriptors(out, bin.descriptors);
    WritePushConstants(out, bin.pushConstants);
}

void ShaderCompiler::WriteDescriptors(std::ofstream& out, const std::vector<Descriptor>& descriptors) const
{
	uint32_t descriptorCount = static_cast<uint32_t>(descriptors.size());
	Core::Write(out, descriptorCount);

    for (const Descriptor& descriptor : descriptors)
    {
        Core::Write(out, descriptor.index);
        size_t nameLen = descriptor.name.size();
        Core::Write(out, nameLen);
        for (size_t i = 0; i < nameLen; ++i)
        {
            Core::Write(out, descriptor.name[i]);
        }
        uint32_t bindingCount = static_cast<uint32_t>(descriptor.bindings.size());
        Core::Write(out, bindingCount);
		for (const Binding& binding : descriptor.bindings)
		{
			Core::Write(out, binding.index);
			Core::Write(out, binding.stage);
			Core::Write(out, binding.count);
			Core::Write(out, binding.type);
			Core::Write(out, binding.shape);
            nameLen = binding.name.size();
            Core::Write(out, nameLen);
            for (size_t i = 0; i < nameLen; ++i)
            {
                Core::Write(out, binding.name[i]);
            }
		}
    }
}

void ShaderCompiler::WritePushConstants(std::ofstream& out, const std::vector<PushConstant>& pushConstants) const
{
    uint32_t pushConstantCount = static_cast<uint32_t>(pushConstants.size());
    Core::Write(out, pushConstantCount);

    for (const PushConstant& pushConstant : pushConstants)
    {
        size_t nameLen = pushConstant.name.size();
        Core::Write(out, nameLen);
        for (size_t i = 0; i < nameLen; ++i)
        {
            Core::Write(out, pushConstant.name[i]);
        }
        Core::Write(out, pushConstant.size);
        Core::Write(out, pushConstant.offset);
        Core::Write(out, pushConstant.stage);
    }
}

void ShaderCompiler::WriteVertexAttributes(std::ofstream& out, const std::vector<VertexAttributeLayout>& vertexAttributeLayouts) const
{
    uint32_t attributesCount = static_cast<uint32_t>(vertexAttributeLayouts.size());
    Core::Write(out, attributesCount);

    for (const VertexAttributeLayout& attribute : vertexAttributeLayouts)
    {
        Core::Write(out, attribute.location);
        Core::Write(out, attribute.binding);
        Core::Write(out, attribute.offset);
        Core::Write(out, attribute.format);
        size_t nameLen = attribute.name.size();
        Core::Write(out, nameLen);
        for (size_t i = 0; i < nameLen; ++i)
        {
            Core::Write(out, attribute.name[i]);
        }
    }
}

void ShaderCompiler::WriteVertexBindings(std::ofstream& out, const std::vector<VertexBindingLayout>& vertexBindingLayouts) const
{
    uint32_t bindingsCount = static_cast<uint32_t>(vertexBindingLayouts.size());
    Core::Write(out, bindingsCount);

    for (const VertexBindingLayout& binding : vertexBindingLayouts)
    {
        Core::Write(out, binding.binding);
        Core::Write(out, binding.stride);
        Core::Write(out, binding.inputRate);
    }
}

void ShaderCompiler::ReadReflectionData(std::ifstream& in, ShaderData& bin, bool isGlobalLayout, bool usesGlobalLayout) const
{
    bin.vertexAttributes = ReadVertexAttributes(in);
    bin.vertexBindings = ReadVertexBindings(in);

    if (!isGlobalLayout && usesGlobalLayout)
        return;

    bin.descriptors = ReadDescriptors(in);
    bin.pushConstants = ReadPushConstants(in);
}

std::vector<Descriptor> ShaderCompiler::ReadDescriptors(std::ifstream& in) const
{
    std::vector<Descriptor> descriptors{};
	uint32_t descriptorCount;

	Core::Read(in, descriptorCount);

	descriptors.resize(descriptorCount);
	for (uint32_t i = 0; i < descriptorCount; ++i)
	{
		Core::Read(in, descriptors[i].index);
        size_t nameLen;
        Core::Read(in, nameLen);
        for (size_t k = 0; k < nameLen; ++k)
        {
            char nameChar;
            Core::Read(in, nameChar);
            descriptors[i].name += nameChar;
        }
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
            Core::Read(in, nameLen);
            for (size_t k = 0; k < nameLen; ++k)
            {
                char nameChar;
                Core::Read(in, nameChar);
                descriptors[i].bindings[j].name += nameChar;
            }
		}
	}
	return descriptors;
}

std::vector<PushConstant> ShaderCompiler::ReadPushConstants(std::ifstream& in) const
{
    std::vector<PushConstant> pushConstants{};
    uint32_t pushConstantCount;

    Core::Read(in, pushConstantCount);

    pushConstants.resize(pushConstantCount);
    for (uint32_t i = 0; i < pushConstantCount; ++i)
    {
        size_t nameLen;
        Core::Read(in, nameLen);
        for (size_t j = 0; j < nameLen; ++j)
        {
            char nameChar;
            Core::Read(in, nameChar);
            pushConstants[i].name += nameChar;
        }
        Core::Read(in, pushConstants[i].size);
        Core::Read(in, pushConstants[i].offset);
        Core::Read(in, pushConstants[i].stage);
    }

    return pushConstants;
}

std::vector<VertexAttributeLayout> ShaderCompiler::ReadVertexAttributes(std::ifstream& in) const
{
    std::vector<VertexAttributeLayout> attributes{};
    uint32_t attributesCount;

    Core::Read(in, attributesCount);

    attributes.resize(attributesCount);
    for (uint32_t i = 0; i < attributesCount; ++i)
    {
        Core::Read(in, attributes[i].location);
        Core::Read(in, attributes[i].binding);
        Core::Read(in, attributes[i].offset);
        Core::Read(in, attributes[i].format);
        size_t nameLen;
        Core::Read(in, nameLen);
        for (size_t k = 0; k < nameLen; ++k)
        {
            char nameChar;
            Core::Read(in, nameChar);
            attributes[i].name += nameChar;
        }
    }

    return attributes;
}

std::vector<VertexBindingLayout> ShaderCompiler::ReadVertexBindings(std::ifstream& in) const
{
    std::vector<VertexBindingLayout> bindings{};
    uint32_t bindingsCount;

    Core::Read(in, bindingsCount);

    bindings.resize(bindingsCount);
    for (uint32_t i = 0; i < bindingsCount; ++i)
    {
        Core::Read(in, bindings[i].binding);
        Core::Read(in, bindings[i].stride);
        Core::Read(in, bindings[i].inputRate);
    }

    return bindings;
}

Shader::~Shader()
{
    for (Descriptor desc : m_descriptors)
        desc.bindings.clear();

    m_descriptors.clear();
    m_vertexAttributes.clear();
    m_vertexBindings.clear();
}

const VertexAttributeLayout Shader::GetVertexAttributeLayout(const std::string name) const
{
    for (const VertexAttributeLayout& Attribute : m_vertexAttributes)
    {
        if (Attribute.name == name)
        {
            return Attribute;
        }
    }

    VertexAttributeLayout fallback;
    fallback.name = "Empty";

    return fallback;
}

END_NAMESPACE_CORE
