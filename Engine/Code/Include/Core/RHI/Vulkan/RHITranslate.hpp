#pragma once

#include "RHI/API/RHI.hpp"
#include "RHI/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

inline Format TranslateFromVulkan(vk::Format format)
{
    switch (format)
    {
    case vk::Format::eB8G8R8A8Srgb:
        return Format::BGRA8_SRGB;

    case vk::Format::eR8G8B8A8Srgb:
        return Format::RGBA8_SRGB;

    case vk::Format::eR8G8B8Srgb:
        return Format::RGB8_SRGB;

    case vk::Format::eR8G8B8A8Unorm:
        return Format::RGBA8_UNORM;

    case vk::Format::eR8G8B8Unorm:
        return Format::RGB8_UNORM;

    case vk::Format::eD32Sfloat:
        return Format::D32_SFLOAT;

    case vk::Format::eD32SfloatS8Uint:
        return Format::D32_SFLOAT_S8_UINT;

    case vk::Format::eD24UnormS8Uint:
        return Format::D24_UNORM_S8_UINT;

    case vk::Format::eUndefined:
    default:
        return Format::Undefined;
    }
}

inline Layout TranslateFromVulkan(vk::ImageLayout layout)
{
    switch (layout)
    {
    case vk::ImageLayout::ePresentSrcKHR:
        return Layout::Present;

    case vk::ImageLayout::eColorAttachmentOptimal:
        return Layout::ColorAttachment;    

    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        return Layout::DepthStencilAttachment;

    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return Layout::ShaderReadOnly;

    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
        return Layout::DepthStencilReadOnly;

    case vk::ImageLayout::eTransferSrcOptimal:
        return Layout::TransferSrc;

    case vk::ImageLayout::eTransferDstOptimal:
        return Layout::TransferDst;

    case vk::ImageLayout::eUndefined:
    default:
        return Layout::Undefined;
    }
}

inline Extent3D TranslateFromVulkan(vk::Extent3D extent)
{
    return Extent3D(extent.width, extent.height, extent.depth);
}

END_NAMESPACE_RHI