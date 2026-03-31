#include "Private/Vulkan/VulkanImage.hpp"


BEGIN_NAMESPACE_RHI

vk::ImageCreateInfo VulkanImage::GetCreateInfo(const ImageSpecs& specs)
{
	// Important for later 
	m_source = specs.source;

	m_finalLayout = TranslateToVulkan(specs.finalLayout);
	m_imageFormat = TranslateToVulkan(specs.format);
	m_imageExtent = TranslateToVulkan(specs.extent);
	m_layersCount = specs.layersCount;
	m_mipLevels = specs.mipLevels;
	m_bytesPerPixel = GetFormatSize(m_imageFormat);

	vk::ImageCreateInfo createInfo;
	createInfo.imageType = TranslateToVulkan(specs.type);
	createInfo.format = m_imageFormat;
	createInfo.extent = m_imageExtent;
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

	m_imageAspects = TranslateToVulkan(specs.viewAspect);

	vk::ImageViewCreateInfo createInfo;
	createInfo.image = m_handle;
	createInfo.viewType = TranslateToVulkan(specs.viewType);
	createInfo.format = m_imageFormat;
	createInfo.subresourceRange.aspectMask = m_imageAspects;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = m_mipLevels;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = m_layersCount;

	return createInfo;
}

uint32_t VulkanImage::GetFormatSize(vk::Format format)
{
	switch (format)
	{
	case vk::Format::eB8G8R8A8Srgb:
	case vk::Format::eR8G8B8A8Srgb:
	case vk::Format::eR8G8B8A8Unorm:
		return 4;

	case vk::Format::eR8G8B8Srgb:
	case vk::Format::eR8G8B8Unorm:
		return 3;

	case vk::Format::eD32Sfloat:
		return 4;

	case vk::Format::eD32SfloatS8Uint:
		return 8;

	case vk::Format::eD24UnormS8Uint:
		return 4;

	default:
		return 0;
	}
}
END_NAMESPACE_RHI