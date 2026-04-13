#pragma once

#include "RHI.hpp"

#include <vector>
#include <string>
#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_RHI

struct Binding
{
    uint32_t index = -1;
    slang::TypeLayoutReflection* typeLayout = nullptr;
    ShaderStage stage{};
    uint32_t count = 0;
};

struct Descriptor
{
    uint32_t index = -1;
    std::vector<Binding> bindings{};
};

struct ShaderData
{
    std::vector<uint8_t> spirv;
#if defined(_WIN32)
    std::vector<uint8_t> dxil;
#endif
    std::vector<Descriptor> descriptors;
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

    ShaderData Load(const std::string& file, const ShaderStage& sType) const;

private:
    ShaderData Compile(const std::string& file, const std::string& content, const std::string& entry) const;
	void Reflect(slang::ProgramLayout* layout) const;

    Slang::ComPtr<slang::IGlobalSession> m_globalSession;
    Slang::ComPtr<slang::ISession> m_session;
};

class Shader : public virtual Core::IResource
{
public:
	virtual ~Shader() = default;

    virtual ShaderStage GetShaderStage() const { return m_type; }

    virtual void SetShaderStage(const ShaderStage& type) { m_type = type; }

protected:
    ShaderStage m_type;
};

END_NAMESPACE_RHI