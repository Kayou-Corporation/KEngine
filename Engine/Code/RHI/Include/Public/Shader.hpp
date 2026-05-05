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

struct VertexAttributeLayout
{
    uint32_t location = 0;
    uint32_t binding = 0;
    uint32_t offset = 0;
    ShaderDataType format = ShaderDataType::Float32_1;
    std::string name = "";

    bool operator==(const VertexAttributeLayout& other) const
    {
        if (name != other.name)
            return false;

        if (location != other.location)
            return false;

        if (binding != other.binding)
            return false;

        if (offset != other.offset)
            return false;

        if (format != other.format)
            return false;

        return true;
    }

    bool operator<(const VertexAttributeLayout& other) const 
    {
        if (location != other.location) 
            return location < other.location;

        if (binding != other.binding) 
            return binding < other.binding;

        if (offset != other.offset) 
            return offset < other.offset;

        return name < other.name;
    }
};

struct VertexBindingLayout
{
    uint32_t binding = 0;
    uint32_t stride = 0;
    VertexInputRate inputRate = VertexInputRate::PerVertex;

    bool operator==(const VertexBindingLayout& other) const 
    {
        return binding == other.binding &&
            stride == other.stride &&
            inputRate == other.inputRate;
    }

    bool operator<(const VertexBindingLayout& other) const 
    {
        if (binding != other.binding) 
            return binding < other.binding;

        if (stride != other.stride) 
            return stride < other.stride;

        return static_cast<int>(inputRate) < static_cast<int>(other.inputRate);
    }
};

struct Binding
{
    uint32_t index = -1;
	slang::BindingType type = slang::BindingType::Unknown;
	SlangResourceShape shape = SLANG_RESOURCE_UNKNOWN;
    ShaderStage stage{};
    uint32_t count = 0;
    std::string name{};
};

struct Descriptor
{
    uint32_t index = -1;
    std::string name{};
    std::vector<Binding> bindings{};

    bool operator==(const Descriptor& other) const
    {
        return index == other.index;
    }

    bool operator<(const Descriptor& other) const
    {
        return index < other.index;
    }
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

    ShaderData Load(const std::string& file, const ShaderStage& stage, bool isGlobalLayout) const;

private:
    ShaderData Compile(const std::string& file, const std::string& content, const std::string& entry, const ShaderStage& stage, bool isGlobalLayout) const;
    static ShaderData Reflect(ShaderData& bin, slang::ProgramLayout* layout, const ShaderStage& stage, bool isGlobalLayout);
    static std::vector<VertexAttributeLayout> ReflectVertexInputs(slang::ProgramLayout* layout);

    static inline ShaderDataType GetFormatFromSlangType(slang::TypeLayoutReflection* typeLayout);
    static inline uint32_t GetFormatSize(ShaderDataType format);

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
    virtual ~Shader();

    virtual ShaderStage GetShaderStage() const { return m_type; }
    virtual const std::vector<Descriptor>& GetDescriptors() const { return m_descriptors; }
    virtual const std::vector<VertexAttributeLayout>& GetVertexAttributes() const { return m_vertexAttributes; }
    virtual const std::vector<VertexBindingLayout>& GetVertexBindings() const { return m_vertexBindings; }

    virtual const VertexAttributeLayout GetVertexAttributeLayout(const std::string name) const;

    virtual void SetShaderStage(const ShaderStage& type) { m_type = type; }
    virtual void SetDescriptors(const std::vector<Descriptor>& descriptors) { m_descriptors = descriptors; }
    virtual void SetVertexAttributes(const std::vector<VertexAttributeLayout>& vertexAttributes) { m_vertexAttributes = vertexAttributes; }
    virtual void SetVertexBindings(const std::vector<VertexBindingLayout>& vertexBindings) { m_vertexBindings = vertexBindings; }

protected:
    ShaderStage m_type{};
	std::vector<Descriptor> m_descriptors{};
    std::vector<VertexAttributeLayout> m_vertexAttributes{};
    std::vector<VertexBindingLayout> m_vertexBindings{};
};

inline ShaderDataType ShaderCompiler::GetFormatFromSlangType(slang::TypeLayoutReflection* typeLayout)
{
    if (!typeLayout)
        return ShaderDataType::Float32_1;

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
        case 1: return ShaderDataType::Float32_1;
        case 2: return ShaderDataType::Float32_2;
        case 3: return ShaderDataType::Float32_3;
        case 4: return ShaderDataType::Float32_4;
        default: return ShaderDataType::Float32_1;
        }
    }
    else if (scalarType == slang::TypeReflection::ScalarType::Int32)
    {
        switch (elementCount)
        {
        case 1: return ShaderDataType::Int32_1;
        case 2: return ShaderDataType::Int32_2;
        case 3: return ShaderDataType::Int32_3;
        case 4: return ShaderDataType::Int32_4;
        default: return ShaderDataType::Int32_1;
        }
    }
    else if (scalarType == slang::TypeReflection::ScalarType::UInt32)
    {
        switch (elementCount)
        {
        case 1: return ShaderDataType::Uint32_1;
        case 2: return ShaderDataType::Uint32_2;
        case 3: return ShaderDataType::Uint32_3;
        case 4: return ShaderDataType::Uint32_4;
        default: return ShaderDataType::Uint32_1;
        }
    }

    return ShaderDataType::Float32_1;
}

inline uint32_t ShaderCompiler::GetFormatSize(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float32_1:
    case ShaderDataType::Int32_1:
    case ShaderDataType::Uint32_1:
        return 4;
    case ShaderDataType::Float32_2:
    case ShaderDataType::Int32_2:
    case ShaderDataType::Uint32_2:
        return 8;
    case ShaderDataType::Float32_3:
    case ShaderDataType::Int32_3:
    case ShaderDataType::Uint32_3:
        return 12;
    case ShaderDataType::Float32_4:
    case ShaderDataType::Int32_4:
    case ShaderDataType::Uint32_4:
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