#pragma once

#include "RHI.hpp"

#include <vector>
#include <string>
#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>
#include <iosfwd>

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"
#include "Utils/File.hpp"

BEGIN_NAMESPACE_RHI

struct Binding
{
    uint32_t index = -1;
	slang::BindingType type = slang::BindingType::Unknown;
	SlangResourceShape shape = SLANG_RESOURCE_UNKNOWN;
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

    ShaderData Load(const std::string& file, const ShaderStage& stage) const;

private:
    ShaderData Compile(const std::string& file, const std::string& content, const std::string& entry, const ShaderStage& stage) const;
    static std::vector<Descriptor> Reflect(slang::ProgramLayout* layout, const ShaderStage& stage);

	void WriteDescriptors(std::ofstream& out, const std::vector<Descriptor>& descriptors) const;
	std::vector<Descriptor> ReadDescriptors(std::ifstream& in) const;

    template <typename T>
    bool CheckIsFileOpenOrValid(T& file, const std::string& name) const;

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
    ShaderStage m_type{};
	std::vector<Descriptor> m_descriptors{};
};

template <typename T>
bool ShaderCompiler::CheckIsFileOpenOrValid(T& file, const std::string& name) const
{
    static_assert((std::is_base_of<std::ifstream, T>::value || std::is_base_of<std::ofstream, T>::value || std::is_base_of<std::fstream, T>::value), "File object is not of right type. Right types are: std::ifstream, std::ofstream and std::fstream");

    if (Core::IsFileOpenOrValid<T>(file))
        return true;

    spdlog::error("Failed to open shader file: {}", name);
    return false;
}

END_NAMESPACE_RHI