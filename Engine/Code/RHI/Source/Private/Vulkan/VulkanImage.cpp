#include "Private/Vulkan/VulkanImage.hpp"


BEGIN_NAMESPACE_RHI

vk::ImageCreateInfo VulkanImage::GetCreateInfo(const ImageSpecs& specs)
{
	// Important for later 
	m_source = specs.source;

	m_finalLayout = TranslateToVulkan(specs.finalLayout);
	m_imageFormat = TranslateToVulkan(specs.format);
	m_layersCount = specs.layersCount;
	m_mipLevels = specs.mipLevels;

	vk::ImageCreateInfo createInfo;
	createInfo.imageType = TranslateToVulkan(specs.type);
	createInfo.format = m_imageFormat;
	createInfo.extent = TranslateToVulkan(specs.extent);
	createInfo.mipLevels = m_mipLevels;
	createInfo.arrayLayers = specs.layersCount;
	createInfo.samples = TranslateToVulkan(specs.sampleCount);
	createInfo.tiling = vk::ImageTiling::eOptimal;

	// Not sure about this but i don't see any other way 
	// Exemple : 
	 // Usage for Color Image for defered : Color & Sampled
	 // Usage for Color Image for defered : Depth & Sampled
	 // Usage Texture Image for defered : TransferDst & Sampled
	vk::ImageUsageFlags usages;
	for (uint32_t i = 0; i < specs.usages.size(); ++i)
	{
		usages |= TranslateToVulkan(specs.usages[i]);
	}
	createInfo.usage = usages;
	
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.initialLayout = vk::ImageLayout::eUndefined;

	return createInfo;
}

vk::ImageViewCreateInfo VulkanImage::GetViewCreateInfo(const ImageSpecs& specs)
{
	(void)specs;

	vk::ImageViewCreateInfo createInfo;
	createInfo.image = m_handle;
	createInfo.viewType = TranslateToVulkan(specs.viewType);
	createInfo.format = m_imageFormat;
	createInfo.subresourceRange.aspectMask = TranslateToVulkan(specs.viewAspect);
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = m_mipLevels;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = m_layersCount;

	return createInfo;
}

END_NAMESPACE_RHI