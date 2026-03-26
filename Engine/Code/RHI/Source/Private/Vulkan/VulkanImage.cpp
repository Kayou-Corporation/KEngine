#include "Private/Vulkan/VulkanImage.hpp"


BEGIN_NAMESPACE_RHI

vk::ImageCreateInfo VulkanImage::GetCreateInfo(const ImageSpecs& specs)
{
	// Important for later 
	m_finalLayout = TranslateToVulkan(specs.finalLayout);
	m_imageFormat = TranslateToVulkan(specs.format);

	vk::ImageCreateInfo createInfo;
	createInfo.imageType = TranslateToVulkan(specs.type);
	createInfo.format = m_imageFormat;
	createInfo.extent = TranslateToVulkan(specs.extent);
	createInfo.mipLevels = specs.mipLevels;
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

	//typedef struct VkImageViewCreateInfo {
	//	VkStructureType            sType;
	//	const void* pNext;
	//	VkImageViewCreateFlags     flags;
	//	VkImage                    image;
	//	VkImageViewType            viewType;
	//	VkFormat                   format;
	//	VkComponentMapping         components;
	//	VkImageSubresourceRange    subresourceRange;
	//} VkImageViewCreateInfo;

	return createInfo;
}

END_NAMESPACE_RHI