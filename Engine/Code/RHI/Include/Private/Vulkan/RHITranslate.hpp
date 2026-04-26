#pragma once

#include "Public/RHI.hpp"
#include "Private/Vulkan/VulkanUtils.hpp"

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

END_NAMESPACE_RHI