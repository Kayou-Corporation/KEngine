#pragma once

#include "Public/Swapchain.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

struct PhysicalDeviceCompatibiliy;

class VulkanSwapchain : public Swapchain
{
public:
	VulkanSwapchain() = default;
	virtual ~VulkanSwapchain() override = default;

	
public:
	void SetHandle(vk::SwapchainKHR swapchain) { m_handle = swapchain; }

	vk::SwapchainKHR GetHandle() { return m_handle; }
	vk::SwapchainKHR& GetHandleRef() { return m_handle; }

	vk::SwapchainCreateInfoKHR GetCreateInfo(const PhysicalDeviceCompatibiliy& compatibility, vk::SurfaceKHR surface, uint32_t graphicsQueueIndex, uint32_t presentQueueQueueIndex, uint32_t requestedImageCount, vk::Format requestedFormat, bool bIsDepthEnable, vk::Format requestedDepthFormat, vk::PresentModeKHR requestedPresentMode, vk::Extent2D requestedExtent);

private:

	static vk::SurfaceFormatKHR FindSuitableFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats, const vk::SurfaceFormatKHR requestedFormat);
	static vk::Extent2D FindSuitableExtent(const vk::SurfaceCapabilitiesKHR& availablecapabilities, const vk::Extent2D  requestedExtent);
	static vk::PresentModeKHR FindSuitablePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentMode, const vk::PresentModeKHR requestedPresentMode);

	vk::SwapchainKHR m_handle;

	uint32_t m_imageCount;

	vk::Format m_colorImageFormat;
	bool m_bIsDepthEnable;
	vk::Format m_depthImageFormat;
};

END_NAMESPACE_RHI