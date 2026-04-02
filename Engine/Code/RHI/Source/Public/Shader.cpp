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

ShaderBinary ShaderCompiler::Load(const std::string& file, const ShaderType& sType)
{
    ShaderBinary bin{};

    std::ifstream fileContent(file, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(fileContent)), std::istreambuf_iterator<char>());

    const std::string entry = ShaderTypeToEntry(sType);
    const std::string hash = HashFile(content, entry);

    const std::string spirvPath = CacheShaderPath(hash, file, ".spv");

    bool pathExists = std::filesystem::exists(spirvPath);

#if defined(_WIN32)
    const std::string dxilPath = CacheShaderPath(hash, file, ".dxil");
    
    pathExists = pathExists && std::filesystem::exists(dxilPath);
#endif

    if (pathExists)
    {
        std::ifstream s(spirvPath, std::ios::binary);
        bin.spirv.assign(std::istreambuf_iterator<char>(s), std::istreambuf_iterator<char>());

#if defined(_WIN32)
        std::ifstream d(dxilPath, std::ios::binary);
        bin.dxil.assign(std::istreambuf_iterator<char>(d), std::istreambuf_iterator<char>());
#endif

        return bin;
    }

    bin = Compile(file, content, entry);

    std::filesystem::create_directories("Cache/Shaders");

    std::ofstream s(spirvPath, std::ios::binary);
    s.write((char*)bin.spirv.data(), bin.spirv.size());

#if defined(_WIN32)
    std::ofstream d(dxilPath, std::ios::binary);
    d.write((char*)bin.dxil.data(), bin.dxil.size());
#endif

    return bin;
}

ShaderBinary ShaderCompiler::Compile(const std::string& file, const std::string& content, const std::string& entry)
{
    ShaderBinary bin{};

    Slang::ComPtr<slang::IModule> slangModule;
    slangModule = m_session->loadModuleFromSourceString(file.c_str(), file.c_str(), content.c_str());

    if (!slangModule)
    {
        spdlog::error("Failed to load module for shader {}", file);
        return {};
    }

    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    slangModule->findEntryPointByName(entry.c_str(), entryPoint.writeRef());

    if (!entryPoint)
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

    m_session->createCompositeComponentType(components, 2, composedProgram.writeRef());

    Slang::ComPtr<slang::IComponentType> linkedProgram;

    composedProgram->link(linkedProgram.writeRef());

#if defined(_WIN32)
    constexpr int nbTargets = 2;
#else
    constexpr int nbTargets = 1;
#endif

    for (int target = 0; target < nbTargets; target++)
    {
        Slang::ComPtr<ISlangBlob> code;

        linkedProgram->getEntryPointCode(0, target, code.writeRef());

        uint8_t* data = (uint8_t*)code->getBufferPointer();

        size_t size = code->getBufferSize();

#if defined(_WIN32)
        if (target == 0)
            bin.spirv.assign(data, data + size);
        else
            bin.dxil.assign(data, data + size);
#else
        bin.spirv.assign(data, data + size);
#endif
    }

    return bin;
}

END_NAMESPACE_RHI