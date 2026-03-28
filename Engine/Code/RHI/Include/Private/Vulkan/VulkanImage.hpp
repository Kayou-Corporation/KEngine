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

	void SetHandle(vk::Image image) { m_handle = image; }
	void SetHandleView(vk::ImageView imageView) { m_handleView = imageView; }

	void SetAllocation(VmaAllocation allocation) { m_allocation = allocation; }
	void SetAllocationInfo(VmaAllocationInfo allocationInfo) { m_allocationInfo = allocationInfo; }

	vk::Image GetHandle() const { return m_handle; }
	vk::Image& GetHandleRef() { return m_handle; }
	vk::ImageView GetHandleView() const { return m_handleView; }
	vk::ImageView& GetHandleViewRef() { return m_handleView; }

private: 
	void ComputeUsage(vk::ImageUsageFlags usages);

private:
	vk::Image m_handle;
	vk::ImageView m_handleView;

	vk::ImageLayout m_finalLayout;
	vk::Format m_imageFormat;
	uint32_t m_layersCount;
	uint32_t m_mipLevels;

	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;
};

END_NAMESPACE_RHI