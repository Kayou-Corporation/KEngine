#pragma once

#include "RHI.hpp"

#include <vector>
#include <string>
#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

BEGIN_NAMESPACE_RHI

struct ShaderBinary
{
    std::vector<uint8_t> spirv;
#if defined(_WIN32)
    std::vector<uint8_t> dxil;
#endif
};

std::string GetShaderName(const std::string& path);

inline std::string CacheShaderPath(const std::string& hash, const std::string& name, const char* ext)
{
    return "Cache/Shaders/" + name + '.' + hash + ext;
}

std::string HashFile(const std::string& content, const std::string& entry);

class ShaderCompiler
{
public:
    void Initialize();

    ShaderBinary Load(const std::string& file, const ShaderType& sType);

private:
    ShaderBinary Compile(const std::string& file, const std::string& content, const std::string& entry);

    Slang::ComPtr<slang::IGlobalSession> m_globalSession;
    Slang::ComPtr<slang::ISession> m_session;
};

END_NAMESPACE_RHI