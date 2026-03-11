#pragma once

#include "Public/Buffer.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanTranslate.hpp"

DISABLE_ALL_WARNINGS

#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_RHI

class VulkanBuffer : public Buffer
{
public:
	VulkanBuffer() = default;
	virtual ~VulkanBuffer() override = default;


public:
	void SetHandle(vk::Buffer buffer) { m_handle = buffer; }

	void SetAllocation(VmaAllocation allocation) { m_allocation = allocation; }
	void SetAllocationInfo(VmaAllocationInfo allocationInfo) { m_allocationInfo = allocationInfo; }

	vk::Buffer GetHandle() { return m_handle; }
	vk::Buffer& GetHandleRef() { return m_handle; }

	VmaAllocation GetAllocation() { return m_allocation; }
	VmaAllocationInfo GetAllocationInfo() { return m_allocationInfo; }

	vk::BufferCreateInfo GetCreateInfo(const BufferSpecs& specs);

private:

	vk::Buffer m_handle;
	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;

};

END_NAMESPACE_RHI