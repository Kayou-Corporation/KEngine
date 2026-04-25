#pragma once

#include <vector>

DISABLE_WARNINGS

#include <vk_mem_alloc.h>

RESTORE_WARNINGS

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>
#include <spdlog/spdlog.h>

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

// ---------- DEBUG LAYERS ---------------
inline const char* TranslateToVulkan(DebugLayers layer)
{
    switch (layer)
    {
    case DebugLayers::Validation:
        return "VK_LAYER_KHRONOS_validation";

    case DebugLayers::DeviceMonitor:
        return "VK_LAYER_LUNARG_monitor";

    case DebugLayers::DeviceSimulation:
        return "VK_LAYER_LUNARG_device_simulation";

    case DebugLayers::APIDump:
        return "VK_LAYER_LUNARG_api_dump";
    }

    return nullptr;
}

inline std::vector<const char*> TranslateToVulkan(const std::vector<DebugLayers>& layers)
{
    std::vector<const char*> vkDebugLayers;

    for (uint32_t i = 0; i < layers.size(); ++i)
    {
        switch (layers[i])
        {
        case DebugLayers::Validation:
            vkDebugLayers.push_back("VK_LAYER_KHRONOS_validation");
            break;

        case DebugLayers::DeviceMonitor:
            vkDebugLayers.push_back("VK_LAYER_LUNARG_monitor");
            break;

        case DebugLayers::DeviceSimulation:
            vkDebugLayers.push_back("VK_LAYER_LUNARG_device_simulation");
            break;

        case DebugLayers::APIDump:
            vkDebugLayers.push_back("VK_LAYER_LUNARG_api_dump");
            break;
        }
    }

    return vkDebugLayers;
}

// ---------- EXTENSIONS ---------------
inline const char* TranslateToVulkan(Extensions extension)
{
    switch (extension)
    {
    case Extensions::Swapchain:
        return "VK_KHR_swapchain";

    case Extensions::DynamicRendering:
        return "VK_KHR_dynamic_rendering";

    case Extensions::ShaderObject:
        return "VK_EXT_shader_object";

    case Extensions::ExtendedDynamicState:
        return "VK_EXT_extended_dynamic_state";

    case Extensions::ExtendedDynamicState2:
        return "VK_EXT_extended_dynamic_state2";
    }

    return nullptr;
}

inline std::vector<const char*> TranslateToVulkan(const std::vector<Extensions>& extension)
{
    std::vector<const char*> vkExtensions;

    for (uint32_t i = 0; i < extension.size(); ++i)
    {
        switch (extension[i])
        {
        case Extensions::Swapchain:
            vkExtensions.push_back("VK_KHR_swapchain");
            break;

        case Extensions::DynamicRendering:
            vkExtensions.push_back("VK_KHR_dynamic_rendering");
            break;

        case Extensions::ShaderObject:
            vkExtensions.push_back("VK_EXT_shader_object");
            break;

        case Extensions::ExtendedDynamicState:
            vkExtensions.push_back("VK_EXT_extended_dynamic_state");
            break;

        case Extensions::ExtendedDynamicState2:
            vkExtensions.push_back("VK_EXT_extended_dynamic_state2");
            break;
        }
    }

    return vkExtensions;
}

// ---------- Features ---------------



// ---------- GPU Type ----------------
inline vk::PhysicalDeviceType TranslateToVulkan(GpuType gpuType)
{
    switch (gpuType)
    {
    case GpuType::Discrete :
        return vk::PhysicalDeviceType::eDiscreteGpu;

    case GpuType::Virtual:
        return vk::PhysicalDeviceType::eVirtualGpu;

    case GpuType::Cpu:
        return vk::PhysicalDeviceType::eCpu;
    }

    return vk::PhysicalDeviceType::eOther;
}

// Queue type
inline vk::QueueFlagBits TranslateToVulkan(QueueType type)
{
    switch (type)
    {
    case QueueType::Graphics:
        return vk::QueueFlagBits::eGraphics;

    case QueueType::Compute:
        return vk::QueueFlagBits::eCompute;

    case QueueType::Transfer:
        return vk::QueueFlagBits::eTransfer;
    }

    return vk::QueueFlagBits::eGraphics;
}

// Format
inline vk::Format TranslateToVulkan(Format format)
{
    switch (format)
    {
    case Format::BGRA8_SRGB:
        return vk::Format::eB8G8R8A8Srgb;

    case Format::RGBA8_SRGB:
        return vk::Format::eR8G8B8A8Srgb;

    case Format::RGB8_SRGB:
        return vk::Format::eR8G8B8Srgb;

    case Format::RGBA8_UNORM:
        return vk::Format::eR8G8B8A8Unorm;

    case Format::RGB8_UNORM:
        return vk::Format::eR8G8B8Unorm;

    case Format::D32_SFLOAT:
        return vk::Format::eD32Sfloat;

    case Format::D32_SFLOAT_S8_UINT:
        return vk::Format::eD32SfloatS8Uint;

    case Format::D24_UNORM_S8_UINT:
        return vk::Format::eD24UnormS8Uint;

    case Format::Undefined:
    default:
        return vk::Format::eUndefined;
    }
}

inline std::vector<vk::Format> TranslateToVulkan(const std::vector<Format>& formatsIn)
{
    std::vector<vk::Format> formatsOut;
    formatsOut.reserve(formatsIn.size());

    for (const auto& f : formatsIn)
    {
        switch (f)
        {
        case Format::BGRA8_SRGB:
            formatsOut.push_back(vk::Format::eB8G8R8A8Srgb);
            break;

        case Format::RGBA8_SRGB:
            formatsOut.push_back(vk::Format::eR8G8B8A8Srgb);
            break;

        case Format::RGB8_SRGB:
            formatsOut.push_back(vk::Format::eR8G8B8Srgb);
            break;

        case Format::RGBA8_UNORM:
            formatsOut.push_back(vk::Format::eR8G8B8A8Unorm);
            break;

        case Format::RGB8_UNORM:
            formatsOut.push_back(vk::Format::eR8G8B8Unorm);
            break;

        case Format::D32_SFLOAT:
            formatsOut.push_back(vk::Format::eD32Sfloat);
            break;

        case Format::D32_SFLOAT_S8_UINT:
            formatsOut.push_back(vk::Format::eD32SfloatS8Uint);
            break;

        case Format::D24_UNORM_S8_UINT:
            formatsOut.push_back(vk::Format::eD24UnormS8Uint);
            break;

        case Format::Undefined:
        default:
            formatsOut.push_back(vk::Format::eUndefined);
            break;
        }
    }
    return formatsOut;
}

inline vk::Format TranslateToVulkan(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float32_1:
        return vk::Format::eR32Sfloat;

    case ShaderDataType::Float32_2:
        return vk::Format::eR32G32Sfloat;

    case ShaderDataType::Float32_3:
        return vk::Format::eR32G32B32Sfloat;

    case ShaderDataType::Float32_4:
        return vk::Format::eR32G32B32A32Sfloat;

    case ShaderDataType::Int32_1:
        return vk::Format::eR32Sint;

    case ShaderDataType::Int32_2:
        return vk::Format::eR32G32Sint;

    case ShaderDataType::Int32_3:
        return vk::Format::eR32G32B32Sint;

    case ShaderDataType::Int32_4:
        return vk::Format::eR32G32B32A32Sint;

    case ShaderDataType::Uint32_1:
        return vk::Format::eR32Uint;

    case ShaderDataType::Uint32_2:
        return vk::Format::eR32G32Uint;

    case ShaderDataType::Uint32_3:
        return vk::Format::eR32G32B32Uint;

    case ShaderDataType::Uint32_4:
        return vk::Format::eR32G32B32A32Uint;

    case ShaderDataType::Undefined:
    default:
        return vk::Format::eUndefined;
    }
}

// ImageUsage
inline vk::ImageUsageFlagBits TranslateToVulkan(ImageUsage usage)
{
    switch (usage)
    {
    case ImageUsage::ColorAttachment :
        return vk::ImageUsageFlagBits::eColorAttachment;

    case ImageUsage::DepthStencilAttachment:
        return vk::ImageUsageFlagBits::eDepthStencilAttachment;

    case ImageUsage::TransferSrc:
        return vk::ImageUsageFlagBits::eTransferSrc;

    case ImageUsage::TransferDst:
        return vk::ImageUsageFlagBits::eTransferDst;

    case ImageUsage::ShaderSampled:
        return vk::ImageUsageFlagBits::eSampled;

    case ImageUsage::Storage:
        return vk::ImageUsageFlagBits::eStorage;

    case ImageUsage::Undefined:
    default:
        return vk::ImageUsageFlagBits::eColorAttachment;
    }
}

// ImageLayout
inline vk::ImageLayout TranslateToVulkan(Layout layout)
{
    switch (layout)
    {
    case Layout::Present:
        return vk::ImageLayout::ePresentSrcKHR;

    case Layout::ColorAttachment:
        return vk::ImageLayout::eColorAttachmentOptimal;

    case Layout::DepthStencilAttachment:
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;

    case Layout::ShaderReadOnly:
        return vk::ImageLayout::eShaderReadOnlyOptimal;

    case Layout::DepthStencilReadOnly:
        return vk::ImageLayout::eDepthStencilReadOnlyOptimal;

    case Layout::TransferSrc:
        return vk::ImageLayout::eTransferSrcOptimal;

    case Layout::TransferDst:
        return vk::ImageLayout::eTransferDstOptimal;

    case Layout::Undefined:
    default:
        return vk::ImageLayout::eUndefined;
    }
}

// ImageLayout
inline vk::ImageType TranslateToVulkan(ImageType type)
{
    switch (type)
    {
    case ImageType::Image1D:
        return vk::ImageType::e1D;

    case ImageType::Image2D:
        return vk::ImageType::e2D;

    case ImageType::Image3D:
        return vk::ImageType::e3D;

    case ImageType::Undefined:
    default:
        return vk::ImageType::e2D;
    }
}

inline vk::ImageViewType TranslateToVulkan(ImageViewType type)
{
    switch (type)
    {
    case ImageViewType::Image1D:
        return vk::ImageViewType::e1D;

    case ImageViewType::Image2D:
        return vk::ImageViewType::e2D;

    case ImageViewType::Image3D:
        return vk::ImageViewType::e3D;

    case ImageViewType::ImageCube:
        return vk::ImageViewType::eCube;

    case ImageViewType::Undefined:
    default:
        return vk::ImageViewType::e2D;
    }
}

inline vk::ImageAspectFlagBits TranslateToVulkan(ImageViewAspect aspect)
{
    switch (aspect)
    {
    case ImageViewAspect::Color:
        return vk::ImageAspectFlagBits::eColor;

    case ImageViewAspect::Depth:
        return vk::ImageAspectFlagBits::eDepth;

    case ImageViewAspect::Undefined:
    default:
        return vk::ImageAspectFlagBits::eNone;
    }
}

// SampleCount
inline vk::SampleCountFlagBits TranslateToVulkan(SampleCount samples)
{
    switch (samples)
    {
    case SampleCount::Count1:
        return vk::SampleCountFlagBits::e1;

    case SampleCount::Count2:
        return vk::SampleCountFlagBits::e2;

    case SampleCount::Count4:
        return vk::SampleCountFlagBits::e4;

    case SampleCount::Count8:
        return vk::SampleCountFlagBits::e8;

    case SampleCount::Count16:
        return vk::SampleCountFlagBits::e16;

    case SampleCount::Count32:
        return vk::SampleCountFlagBits::e32;

    case SampleCount::Count64:
        return vk::SampleCountFlagBits::e64;

    default:
        return vk::SampleCountFlagBits::e1;
    }
}

inline vk::PresentModeKHR TranslateToVulkan(PresentMode mode)
{
    switch (mode)
    {
    case PresentMode::Immediate:
        return vk::PresentModeKHR::eImmediate;

    case PresentMode::Mailbox:
        return vk::PresentModeKHR::eMailbox;

    case PresentMode::Fifo:
        return vk::PresentModeKHR::eFifo;

    case PresentMode::FifoRelaxed:
        return vk::PresentModeKHR::eFifoRelaxed;

    case PresentMode::Undefined:
    default:
        return vk::PresentModeKHR::eFifo;
    }
}

inline vk::Extent2D TranslateToVulkan(const Extent2D& extent)
{
    return vk::Extent2D
    {
        extent.x,
        extent.y
    };
}

inline vk::Offset2D TranslateToVulkan(const Offset2D& extent)
{
    return vk::Offset2D
    {
        extent.x,
        extent.y
    };
}

inline vk::Extent3D TranslateToVulkan(const Extent3D& extent)
{
    return vk::Extent3D
    {
        extent.x,
        extent.y,
        extent.z
    };
}

// Buffer usage
inline vk::BufferUsageFlags TranslateToVulkan(BufferUsage usage)
{
    switch (usage)
    {
    case BufferUsage::Vertex:
        return vk::BufferUsageFlagBits::eVertexBuffer;

    case BufferUsage::Index:
        return vk::BufferUsageFlagBits::eIndexBuffer;

    case BufferUsage::Storage:
        return vk::BufferUsageFlagBits::eStorageBuffer;

    case BufferUsage::Uniform:
        return vk::BufferUsageFlagBits::eUniformBuffer;

    case BufferUsage::TransferSrc:
        return vk::BufferUsageFlagBits::eTransferSrc;

    case BufferUsage::TransferDst:
        return vk::BufferUsageFlagBits::eTransferDst;

    default:
        return {};
    }
}

inline vk::BufferUsageFlags TranslateToVulkan(
    const std::vector<BufferUsage>& usages)
{
    vk::BufferUsageFlags flags{};

    for (auto usage : usages)
    {
        switch (usage)
        {
        case BufferUsage::Vertex:
            flags |= vk::BufferUsageFlagBits::eVertexBuffer;
            break;

        case BufferUsage::Index:
            flags |= vk::BufferUsageFlagBits::eIndexBuffer;
            break;

        case BufferUsage::Storage:
            flags |= vk::BufferUsageFlagBits::eStorageBuffer;
            break;

        case BufferUsage::Uniform:
            flags |= vk::BufferUsageFlagBits::eUniformBuffer;
            break;

        case BufferUsage::TransferSrc:
            flags |= vk::BufferUsageFlagBits::eTransferSrc;
            break;

        case BufferUsage::TransferDst:
            flags |= vk::BufferUsageFlagBits::eTransferDst;
            break;
        }
    }

    return flags;
}

// Memory access
inline VmaAllocationCreateInfo TranslateToVulkan(MemoryAccess access)
{
    VmaAllocationCreateInfo createInfo{};

    switch (access)
    {
    case MemoryAccess::CPU_Read:
        createInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        createInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        break;

    case MemoryAccess::CPU_Write:
        createInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        createInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        break;

    case MemoryAccess::GPU_Only:
        createInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    }

    return createInfo;
}

// Pipeline stage
inline vk::PipelineStageFlagBits TranslateToVulkan(PipelineStage stage)
{
    switch (stage)
    {
    case PipelineStage::VertexInput:
        return vk::PipelineStageFlagBits::eVertexInput;

    case PipelineStage::VertexShader:
        return vk::PipelineStageFlagBits::eVertexShader;

    case PipelineStage::FragmentShader:
        return vk::PipelineStageFlagBits::eFragmentShader;

    case PipelineStage::ComputeShader:
        return vk::PipelineStageFlagBits::eComputeShader;

    case PipelineStage::GeometryShader:
        return vk::PipelineStageFlagBits::eGeometryShader;
        
    case PipelineStage::Transfer:
        return vk::PipelineStageFlagBits::eTransfer;

    case PipelineStage::ColorOutput:
        return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case PipelineStage::None:
    default:
        return vk::PipelineStageFlagBits::eNone;
    }
}

inline vk::ShaderStageFlagBits TranslateToVulkan(ShaderStage stage)
{
    switch (stage)
    {
    case ShaderStage::Vertex:
        return vk::ShaderStageFlagBits::eVertex;
    case ShaderStage::Fragment:
        return vk::ShaderStageFlagBits::eFragment;
    case ShaderStage::Compute:
        return vk::ShaderStageFlagBits::eCompute;
    case ShaderStage::Geometry:
        return vk::ShaderStageFlagBits::eGeometry;
    case ShaderStage::Tesselation: // Tesselation not supported for now
        spdlog::error("Unsupported shader stage: tessellation");
        return static_cast<vk::ShaderStageFlagBits>(0);
    default:
        spdlog::error("Unknown shader stage");
        return static_cast<vk::ShaderStageFlagBits>(0);
    }
}

inline vk::VertexInputRate TranslateToVulkan(VertexInputRate rate)
{
    switch (rate)
    {
    case VertexInputRate::PerVertex:
        return vk::VertexInputRate::eVertex;
    case VertexInputRate::PerInstance:
        return vk::VertexInputRate::eInstance;
    default:
        return vk::VertexInputRate::eVertex;
    }
}

inline vk::DescriptorType TranslateToVulkan(const slang::BindingType bindingType, const SlangResourceShape shape)
{
    switch (bindingType)
    {
    case slang::BindingType::ConstantBuffer:
    case slang::BindingType::ParameterBlock:
        return vk::DescriptorType::eUniformBuffer;

    case slang::BindingType::Sampler:
        return vk::DescriptorType::eSampler;

    case slang::BindingType::Texture:
    {
        switch (shape)
        {
        case SLANG_TEXTURE_1D:
        case SLANG_TEXTURE_2D:
        case SLANG_TEXTURE_3D:
        case SLANG_TEXTURE_CUBE:
        case SLANG_TEXTURE_1D_ARRAY:
        case SLANG_TEXTURE_2D_ARRAY:
        case SLANG_TEXTURE_CUBE_ARRAY:
            return vk::DescriptorType::eSampledImage;

        default:
            return vk::DescriptorType::eSampledImage;
        }
    }

    case slang::BindingType::CombinedTextureSampler:
        return vk::DescriptorType::eCombinedImageSampler;

    case slang::BindingType::MutableTexture:
    case slang::BindingType::RawBuffer:
    case slang::BindingType::TypedBuffer:
    case slang::BindingType::MutableTypedBuffer:
    case slang::BindingType::MutableRawBuffer:
        return vk::DescriptorType::eStorageBuffer;

    case slang::BindingType::RayTracingAccelerationStructure:
        return vk::DescriptorType::eAccelerationStructureKHR;

    default:
        spdlog::error("Unsupported Slang binding type");
        return vk::DescriptorType::eUniformBuffer;
    }
}

inline vk::PrimitiveTopology TranslateToVulkan(PrimitiveTopology topology)
{
    switch (topology)
    {
    case PrimitiveTopology::TriangleList:
        return vk::PrimitiveTopology::eTriangleList;

    case PrimitiveTopology::TriangleStrip:
        return vk::PrimitiveTopology::eTriangleStrip;

    default:
        return vk::PrimitiveTopology::eTriangleList;
    }
}

inline std::vector<vk::DynamicState> TranslateToVulkan(const std::vector<DynamicState>& statesIn)
{
    std::vector<vk::DynamicState> statesOut;
    statesOut.reserve(statesIn.size());

    for (const auto& s : statesIn)
    {
        switch (s)
        {
        case DynamicState::ViewPort:
            statesOut.push_back(vk::DynamicState::eViewport);
            break;
        case DynamicState::Scissor:
            statesOut.push_back(vk::DynamicState::eScissor);
            break;
        }
    }
    return statesOut;
}

inline vk::FrontFace TranslateToVulkan(FrontFace face)
{
    switch (face)
    {
    case FrontFace::ClockWise:
        return vk::FrontFace::eClockwise;
    case FrontFace::CounterClockWise:
        return vk::FrontFace::eCounterClockwise;
    default:
        return vk::FrontFace::eCounterClockwise;
    }
}

inline vk::CullModeFlags TranslateToVulkan(CullMode mode)
{
    switch (mode)
    {
    case CullMode::Front:
        return vk::CullModeFlagBits::eFront;
    case CullMode::Back:
        return vk::CullModeFlagBits::eBack;
    default:
        return vk::CullModeFlagBits::eNone;
    }
    
    return vk::CullModeFlagBits::eNone; 
}

inline vk::AttachmentStoreOp TranslateToVulkan(StoreOp storeOp)
{
    switch (storeOp)
    {
    case StoreOp::Store:
        return vk::AttachmentStoreOp::eStore;

    case StoreOp::DontCare:
        return vk::AttachmentStoreOp::eDontCare;

    default:
        return vk::AttachmentStoreOp::eNone;
    }
}

inline vk::AttachmentLoadOp TranslateToVulkan(LoadOp storeOp)
{
    switch (storeOp)
    {
    case LoadOp::Load:
        return vk::AttachmentLoadOp::eLoad;

    case LoadOp::DontCare:
        return vk::AttachmentLoadOp::eDontCare;

    case LoadOp::Clear:
        return vk::AttachmentLoadOp::eClear;

    default:
        return vk::AttachmentLoadOp::eNone;
    }
}

END_NAMESPACE_RHI