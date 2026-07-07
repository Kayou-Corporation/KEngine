#pragma once

#include "RHI/API/Image.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"
#include "RHI/Vulkan/VulkanTranslate.hpp"

DISABLE_WARNINGS

#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_RHI

class VulkanImage : public Image
{
public:
	VulkanImage() = default;
	virtual ~VulkanImage() override = default;

public:
	// Get create info
	vk::ImageCreateInfo GetCreateInfo(const ImageSpecs& specs);
	vk::ImageViewCreateInfo GetViewCreateInfo(const ImageSpecs& specs);
	vk::ImageViewCreateInfo GetViewCreateInfoForPresentation(vk::Format format, vk::Extent3D extent);
	vk::ImageCreateInfo GetCreateInfoForSwapchain(SwapchainImageSpecs specs, vk::Format format, vk::Extent3D extent);
	vk::ImageViewCreateInfo GetViewCreateInfoForSwapchain(SwapchainImageSpecs specs);

	// Setter
	void SetHandle(vk::Image image) { m_handle = image; }
	void SetHandleView(vk::ImageView imageView) { m_handleView = imageView; }
	void SetCurrentLayout(vk::ImageLayout layout);
	void SetTargetLayout(vk::ImageLayout layout);

	void SetAllocation(VmaAllocation allocation) { m_allocation = allocation; }
	void SetAllocationInfo(VmaAllocationInfo allocationInfo) { m_allocationInfo = allocationInfo; }

	// Setter
	vk::Image GetHandle() const { return m_handle; }
	vk::Image& GetHandleRef() { return m_handle; }
	vk::ImageView GetHandleView() const { return m_handleView; }
	vk::ImageView& GetHandleViewRef() { return m_handleView; }

	vk::Extent3D GetVulkanExtent() { return m_imageExtent; }
	vk::Format GetVulkanImageFormat() { return m_imageFormat; }
	vk::ImageLayout GetVulkanCurrentLayout() { return m_imageLayout; }
	vk::ImageLayout GetVulkanTargetLayout() { return m_targetLayout; }

	uint32_t GetLayersCount() { return m_layersCount; }
	uint32_t GetMipLevels() { return m_mipLevels; }
	uint32_t GetBytesPerPixel() { return m_bytesPerPixel; }
	vk::ImageAspectFlags GetAspect() { return m_imageAspects; }

	VmaAllocation GetAllocation() { return  m_allocation; }
private: 
	void ComputeUsage(vk::ImageUsageFlags usages);
	uint32_t GetFormatSize(vk::Format format);

private:
	vk::Image m_handle;
	vk::ImageView m_handleView;

	// Current data, only use in vulkan context
	vk::ImageLayout m_imageLayout = vk::ImageLayout::eUndefined;
	vk::Format m_imageFormat;
	vk::ImageAspectFlags m_imageAspects;
	vk::Extent3D m_imageExtent;

	vk::ImageLayout m_targetLayout;

	uint32_t m_layersCount;
	uint32_t m_mipLevels;
	uint32_t m_bytesPerPixel;

	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;
};

END_NAMESPACE_RHI