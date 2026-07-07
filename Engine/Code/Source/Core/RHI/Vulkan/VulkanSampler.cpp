#include "RHI/Vulkan/VulkanSampler.hpp"

#include "RHI/Vulkan/VulkanTranslate.hpp"

#include <algorithm>

BEGIN_NAMESPACE_CORE

vk::SamplerCreateInfo VulkanSampler::GetCreateInfo(SamplerSpecs specs, float maxDeviceAnisotropy)
{
    vk::SamplerCreateInfo createInfo{};

    createInfo.magFilter = TranslateToVulkan(specs.magFilter);
    createInfo.minFilter = TranslateToVulkan(specs.minFilter);
    createInfo.mipmapMode = TranslateToVulkan(specs.mipmapMode);
    createInfo.addressModeU = TranslateToVulkan(specs.addressU);
    createInfo.addressModeV = TranslateToVulkan(specs.addressV);
    createInfo.addressModeW = TranslateToVulkan(specs.addressW);
    createInfo.anisotropyEnable = specs.anisotropyEnable ? VK_TRUE : VK_FALSE;
    createInfo.maxAnisotropy = std::clamp(specs.maxAnisotropy, 1.0f, maxDeviceAnisotropy);
    createInfo.compareEnable = specs.compareEnable ? VK_TRUE : VK_FALSE;
    createInfo.compareOp = TranslateToVulkan(specs.compareOp);
    createInfo.minLod = specs.minLod;
    createInfo.maxLod = specs.maxLod;
    createInfo.mipLodBias = specs.mipLodBias;
    createInfo.borderColor = TranslateToVulkan(specs.borderColor);
    createInfo.unnormalizedCoordinates = specs.unnormalizedCoordinates ? VK_TRUE : VK_FALSE;

    return createInfo;
}

END_NAMESPACE_CORE