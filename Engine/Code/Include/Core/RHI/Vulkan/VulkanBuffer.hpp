#pragma once

#include "RHI/API/Buffer.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"
#include "RHI/Vulkan/VulkanTranslate.hpp"

DISABLE_WARNINGS

#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_CORE

class VulkanBuffer : public Buffer
{
public:
	VulkanBuffer() = default;
	virtual ~VulkanBuffer() override = default;

	virtual void* GetMappedData() const override { return m_allocationInfo.pMappedData; }

public:
	vk::AccessFlags GetAccessMask();

	void SetHandle(vk::Buffer buffer) { m_handle = buffer; }

	void SetAllocation(VmaAllocation allocation) { m_allocation = allocation; }
	void SetAllocationInfo(VmaAllocationInfo allocationInfo) { m_allocationInfo = allocationInfo; }

	void SetSize(uint32_t size) { m_size = size; }

	void SetIsGpuOnly(bool isGpuOnly) { m_isGpuOnly = isGpuOnly; }

	vk::Buffer GetHandle() const { return m_handle; }
	vk::Buffer& GetHandleRef() { return m_handle; }

	VmaAllocation GetAllocation() const { return m_allocation; }
	VmaAllocationInfo GetAllocationInfo() const { return m_allocationInfo; }

	vk::BufferUsageFlags GetPrimaryUsage() const { return m_primaryUsage; }
	vk::BufferUsageFlags GetUsage() const { return m_usage; }
	vk::PipelineStageFlagBits GetPipelineStage() const { return m_pipelineStage; }

	vk::BufferCreateInfo GetCreateInfo(const BufferSpecs& specs);

private:

	vk::Buffer m_handle;
	VmaAllocation m_allocation;
	VmaAllocationInfo m_allocationInfo;
	vk::BufferUsageFlags m_primaryUsage;
	vk::BufferUsageFlags m_usage;
	vk::PipelineStageFlagBits m_pipelineStage;

};

// It will probably be useful to implement more flags later...
inline vk::AccessFlags GetAccessFlagsFromUsage(vk::BufferUsageFlags usage)
{
	vk::AccessFlags flags{};

	if (usage & vk::BufferUsageFlagBits::eVertexBuffer)
		flags |= vk::AccessFlagBits::eVertexAttributeRead;

	if (usage & vk::BufferUsageFlagBits::eIndexBuffer)
		flags |= vk::AccessFlagBits::eIndexRead;

	if (usage & vk::BufferUsageFlagBits::eUniformBuffer)
		flags |= vk::AccessFlagBits::eUniformRead;

	if (usage & vk::BufferUsageFlagBits::eStorageBuffer)
		flags |= vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;

	if (usage & vk::BufferUsageFlagBits::eTransferSrc)
		flags |= vk::AccessFlagBits::eTransferRead;

	if (usage & vk::BufferUsageFlagBits::eTransferDst)
		flags |= vk::AccessFlagBits::eTransferWrite;

	return flags;
}

inline vk::AccessFlags GetWriteAccessFlagsFromUsage(vk::BufferUsageFlags usage)
{
	vk::AccessFlags flags{};

	if (usage & vk::BufferUsageFlagBits::eStorageBuffer)
		flags |= vk::AccessFlagBits::eShaderWrite;

	if (usage & vk::BufferUsageFlagBits::eTransferDst)
		flags |= vk::AccessFlagBits::eTransferWrite;

	return flags;
}

END_NAMESPACE_CORE