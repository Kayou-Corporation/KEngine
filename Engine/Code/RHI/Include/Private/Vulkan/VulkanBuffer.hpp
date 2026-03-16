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

	void* GetMappedData() const override { return m_allocationInfo.pMappedData; }

public:
	void SetHandle(vk::Buffer buffer) { m_handle = buffer; }

	void SetAllocation(VmaAllocation allocation) { m_allocation = allocation; }
	void SetAllocationInfo(VmaAllocationInfo allocationInfo) { m_allocationInfo = allocationInfo; }

	void SetSize(uint32_t size) { m_size = size; }

	void SetIsGpuOnly(bool isGpuOnly) { m_isGpuOnly = isGpuOnly; }

	vk::Buffer GetHandle() const { return m_handle; }
	vk::Buffer& GetHandleRef() { return m_handle; }

	VmaAllocation GetAllocation() const { return m_allocation; }
	VmaAllocationInfo GetAllocationInfo() const { return m_allocationInfo; }

	vk::PipelineStageFlagBits GetPipelineStage() const { return m_pipelineStage; }

	vk::BufferCreateInfo GetCreateInfo(const BufferSpecs& specs);

private:

	vk::Buffer m_handle;
	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;
	vk::BufferUsageFlags m_primaryUsage;
	vk::PipelineStageFlagBits m_pipelineStage;

};

END_NAMESPACE_RHI