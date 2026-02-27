#include "Core/RHI/Private/Vulkan/VulkanSwapchain.hpp"
#include "Core/RHI/Private/Vulkan/VulkanDevice.hpp"
#include <algorithm>
#include <iostream>


vk::SwapchainCreateInfoKHR VulkanSwapchain::GetCreateInfo(const PhysicalDeviceCompatibiliy& compatibility, vk::SurfaceKHR surface, uint32_t graphicsQueueIndex, uint32_t presentQueueQueueIndex, uint32_t requestedImageCount, vk::Format requestedFormat, bool bIsDepthEnable, vk::Format requestedDepthFormat, vk::PresentModeKHR requestedPresentMode, vk::Extent2D requestedExtent)
{
	vk::SurfaceFormatKHR requestedSurfaceFormat{};
	requestedSurfaceFormat.format = requestedFormat;
	requestedSurfaceFormat.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
	const vk::SurfaceFormatKHR surfaceFormat = VulkanSwapchain::FindSuitableFormat(compatibility.formats, requestedSurfaceFormat);

	const vk::PresentModeKHR presentMode = VulkanSwapchain::FindSuitablePresentMode(compatibility.presentModes, requestedPresentMode);
	const vk::Extent2D extent = VulkanSwapchain::FindSuitableExtent(compatibility.capabilities, requestedExtent);

	uint32_t imageCount = 0;
	if (requestedImageCount <= 0)
	{
		imageCount = compatibility.capabilities.minImageCount + 1;

		if (compatibility.capabilities.maxImageCount > 0 && imageCount > compatibility.capabilities.maxImageCount)
		{
			imageCount = compatibility.capabilities.maxImageCount;
		}
		spdlog::warn("The requested number of image for the swapchain is not available, default value use");
	}
	else
	{
		imageCount = requestedImageCount;
	}

	vk::SharingMode sharingMode = vk::SharingMode::eExclusive;
	uint32_t queueFamilyCount = 0;
	uint32_t queueFamilyIndices[2] = {};

	if (graphicsQueueIndex != presentQueueQueueIndex)
	{
		sharingMode = vk::SharingMode::eConcurrent;
		queueFamilyCount = 2;
		queueFamilyIndices[0] = graphicsQueueIndex;
		queueFamilyIndices[1] = presentQueueQueueIndex;
	}

	vk::SwapchainCreateInfoKHR createInfo{};
	createInfo.surface = surface;

	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;	
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	createInfo.minImageCount = imageCount;

	createInfo.imageSharingMode = sharingMode;
	createInfo.queueFamilyIndexCount = queueFamilyCount;
	createInfo.pQueueFamilyIndices = queueFamilyIndices;

	createInfo.preTransform = compatibility.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	
	m_imageCount = imageCount;
	m_colorImageFormat = surfaceFormat.format;
	m_bIsDepthEnable = bIsDepthEnable;
	m_depthImageFormat = requestedDepthFormat;

	return createInfo;
}

#pragma region Compatibility

vk::SurfaceFormatKHR VulkanSwapchain::FindSuitableFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats, const vk::SurfaceFormatKHR requestedFormat)
{
	ASSERT(!availableFormats.empty(), "No available formats !");

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == requestedFormat.format)
		{
			return availableFormat;
		}
	}

	spdlog::warn("Requested surface format unavailable, use first format available instead");
	return availableFormats[0];
}

vk::Extent2D VulkanSwapchain::FindSuitableExtent(const vk::SurfaceCapabilitiesKHR& availablecapabilities, const vk::Extent2D  requestedExtent)
{
	if (availablecapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return availablecapabilities.currentExtent;
	}
	
	VkExtent2D extent
	{
		.width = std::clamp(requestedExtent.width, availablecapabilities.minImageExtent.width, availablecapabilities.maxImageExtent.width),
		.height = std::clamp(requestedExtent.height, availablecapabilities.minImageExtent.height, availablecapabilities.maxImageExtent.height)
	};
	
	return extent;
}

vk::PresentModeKHR VulkanSwapchain::FindSuitablePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentMode, const vk::PresentModeKHR requestedPresentMode)
{
	ASSERT(!availablePresentMode.empty(), "No available formats !");
	
	for (const auto& availablePresentMode : availablePresentMode)
	{
		if (availablePresentMode == requestedPresentMode)
		{
			return availablePresentMode;
		}
	}
	spdlog::warn("Requested present mode unavailable, use default present mode instead");
	return vk::PresentModeKHR::eFifo;
}

#pragma endregion