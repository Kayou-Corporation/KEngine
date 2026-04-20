#pragma once

#include "Public/Image.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanTranslate.hpp"

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
	vk::ImageCreateInfo GetCreateInfo(const ImageSpecs& specs);
	vk::ImageViewCreateInfo GetViewCreateInfo(const ImageSpecs& specs);

	vk::ImageViewCreateInfo GetViewCreateInfoForPresentation(vk::Format format, vk::Extent3D extent);

	vk::ImageCreateInfo GetCreateInfoForSwapchain(SwapchainImageSpecs specs, vk::Format format, vk::Extent3D extent);
	vk::ImageViewCreateInfo GetViewCreateInfoForSwapchain(SwapchainImageSpecs specs);

	void SetHandle(vk::Image image) { m_handle = image; }
	void SetHandleView(vk::ImageView imageView) { m_handleView = imageView; }
	void SetLayout(vk::ImageLayout layout) { m_layout = layout; }

	void SetAllocation(VmaAllocation allocation) { m_allocation = allocation; }
	void SetAllocationInfo(VmaAllocationInfo allocationInfo) { m_allocationInfo = allocationInfo; }

	vk::Image GetHandle() const { return m_handle; }
	vk::Image& GetHandleRef() { return m_handle; }
	vk::ImageView GetHandleView() const { return m_handleView; }
	vk::ImageView& GetHandleViewRef() { return m_handleView; }

	uint32_t GetLayersCount() { return m_layersCount; }
	uint32_t GetMipLevels() { return m_mipLevels; }
	uint32_t GetBytesPerPixel() { return m_bytesPerPixel; }
	vk::Extent3D GetExtent() { return m_imageExtent; }
	vk::ImageAspectFlags GetAspect() { return m_imageAspects; }
	vk::ImageLayout GetFinalLayout() { return m_finalLayout; }
	vk::ImageLayout GetLayout() { return m_layout; }

	VmaAllocation GetAllocation() { return  m_allocation; }
private: 
	void ComputeUsage(vk::ImageUsageFlags usages);
	uint32_t GetFormatSize(vk::Format format);

private:
	vk::Image m_handle;
	vk::ImageView m_handleView;

	// Target = Main image use, Final = Final image use)
	// Ex : texture : Target & Final = ShaderReadOnly
	// Ex : SwapchainColor : Target : ColorOutput, Final = Present
	// Ex : PresentationDepth : Target & Final = DepthOutput
	vk::ImageLayout m_targetLayout;
	vk::ImageLayout m_finalLayout;
	vk::ImageLayout m_layout;
	vk::Format m_imageFormat;
	vk::Extent3D m_imageExtent;
	vk::ImageAspectFlags m_imageAspects;

	uint32_t m_layersCount;
	uint32_t m_mipLevels;
	uint32_t m_bytesPerPixel;

	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;
};

END_NAMESPACE_RHI