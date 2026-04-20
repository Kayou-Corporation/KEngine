#pragma once

#include "RHI.hpp"

#include <vector>
#include <string>
#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>
#include <iosfwd>
#include <string>

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"
#include "Utils/File.hpp"

BEGIN_NAMESPACE_RHI

enum class VertexFormat : uint8_t
{
    Float32_1,     // float
    Float32_2,     // vec2
    Float32_3,     // vec3
    Float32_4,     // vec4
    Int32_1,       // int
    Int32_2,       // ivec2
    Int32_3,       // ivec3
    Int32_4,       // ivec4
    Uint32_1,      // uint
    Uint32_2,      // uvec2
    Uint32_3,      // uvec3
    Uint32_4,      // uvec4
};

enum class VertexInputRate : uint8_t
{
    PerVertex,
    PerInstance
};

struct VertexAttributeLayout
{
    uint32_t location = 0;
    uint32_t binding = 0;
    uint32_t offset = 0;
    VertexFormat format = VertexFormat::Float32_1;
    std::string name = "";
};

struct VertexBindingLayout
{
    uint32_t binding = 0;
    uint32_t stride = 0;
    VertexInputRate inputRate = VertexInputRate::PerVertex;
};

struct Binding
{
    uint32_t index = -1;
	slang::BindingType type = slang::BindingType::Unknown;
	SlangResourceShape shape = SLANG_RESOURCE_UNKNOWN;
    ShaderStage stage{};
    uint32_t count = 0;
    std::string name = "";
};

struct Descriptor
{
    uint32_t index = -1;
    std::vector<Binding> bindings{};
};

struct ShaderData
{
    std::vector<uint8_t> spirv{};
#if defined(_WIN32)
    std::vector<uint8_t> dxil{};
#endif
    std::vector<Descriptor> descriptors{};
    std::vector<VertexAttributeLayout> vertexAttributes{};
    std::vector<VertexBindingLayout> vertexBindings{};
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
    static ShaderData Reflect(ShaderData& bin, slang::ProgramLayout* layout, const ShaderStage& stage);
    static std::vector<VertexAttributeLayout> ReflectVertexInputs(slang::ProgramLayout* layout);

    static inline VertexFormat GetVertexFormatFromSlangType(slang::TypeLayoutReflection* typeLayout);
    static inline uint32_t GetVertexFormatSize(VertexFormat format);

	void WriteReflectionData(std::ofstream& out, const ShaderData& bin) const;
	void WriteDescriptors(std::ofstream& out, const std::vector<Descriptor>& descriptors) const;
	void WriteVertexAttributes(std::ofstream& out, const std::vector<VertexAttributeLayout>& vertexAttributeLayouts) const;
	void WriteVertexBindings(std::ofstream& out, const std::vector<VertexBindingLayout>& vertexBindingLayouts) const;

	void ReadReflectionData(std::ifstream& in, ShaderData& bin) const;
	std::vector<Descriptor> ReadDescriptors(std::ifstream& in) const;
	std::vector<VertexAttributeLayout> ReadVertexAttributes(std::ifstream& in) const;
	std::vector<VertexBindingLayout> ReadVertexBindings(std::ifstream& in) const;

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
    virtual void SetDescriptors(const std::vector<Descriptor>& descriptors) { m_descriptors = descriptors; }

protected:
    ShaderStage m_type{};
	std::vector<Descriptor> m_descriptors{};
};

inline VertexFormat ShaderCompiler::GetVertexFormatFromSlangType(slang::TypeLayoutReflection* typeLayout)
{
    if (!typeLayout)
        return VertexFormat::Float32_1;

    auto kind = typeLayout->getKind();
    auto scalarType = typeLayout->getScalarType();

    uint32_t elementCount = 1;
    if (kind == slang::TypeReflection::Kind::Vector)
    {
        elementCount = static_cast<uint32_t>(typeLayout->getElementCount());
    }

    if (scalarType == slang::TypeReflection::ScalarType::Float32)
    {
        switch (elementCount)
        {
        case 1: return VertexFormat::Float32_1;
        case 2: return VertexFormat::Float32_2;
        case 3: return VertexFormat::Float32_3;
        case 4: return VertexFormat::Float32_4;
        default: return VertexFormat::Float32_1;
        }
    }
    else if (scalarType == slang::TypeReflection::ScalarType::Int32)
    {
        switch (elementCount)
        {
        case 1: return VertexFormat::Int32_1;
        case 2: return VertexFormat::Int32_2;
        case 3: return VertexFormat::Int32_3;
        case 4: return VertexFormat::Int32_4;
        default: return VertexFormat::Int32_1;
        }
    }
    else if (scalarType == slang::TypeReflection::ScalarType::UInt32)
    {
        switch (elementCount)
        {
        case 1: return VertexFormat::Uint32_1;
        case 2: return VertexFormat::Uint32_2;
        case 3: return VertexFormat::Uint32_3;
        case 4: return VertexFormat::Uint32_4;
        default: return VertexFormat::Uint32_1;
        }
    }

    return VertexFormat::Float32_1;
}

inline uint32_t ShaderCompiler::GetVertexFormatSize(VertexFormat format)
{
    switch (format)
    {
    case VertexFormat::Float32_1:
    case VertexFormat::Int32_1:
    case VertexFormat::Uint32_1:
        return 4;
    case VertexFormat::Float32_2:
    case VertexFormat::Int32_2:
    case VertexFormat::Uint32_2:
        return 8;
    case VertexFormat::Float32_3:
    case VertexFormat::Int32_3:
    case VertexFormat::Uint32_3:
        return 12;
    case VertexFormat::Float32_4:
    case VertexFormat::Int32_4:
    case VertexFormat::Uint32_4:
        return 16;
    default:
        return 0;
    }
}

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