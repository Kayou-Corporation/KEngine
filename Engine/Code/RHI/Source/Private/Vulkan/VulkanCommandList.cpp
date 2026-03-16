#include "Private/Vulkan/VulkanCommandList.hpp"

#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanDevice.hpp"

DISABLE_ALL_WARNINGS

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_RHI

void VulkanCommandList::Open()
{
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	VK_CHECK_VOID(m_handle->cmdBuffer.begin(beginInfo), "Can't begin command buffer");
}

void VulkanCommandList::Close()
{
	VK_CHECK_VOID(m_handle->cmdBuffer.end(),"Can't end command buffer");
}

void VulkanCommandList::SetBufferData(Core::RefCountPtr<Buffer> buffer, void* data, uint32_t size, uint32_t offset, Core::RefCountPtr<Device> device)
{
	ASSERT((size + offset) <= buffer->GetSize(), "Data is too large");
	// TODO : Implement this
	(void)data;

	Core::RefCountPtr<VulkanDevice> vulkanDevice = device.CastAs<VulkanDevice>();
	Core::RefCountPtr<VulkanBuffer> vulkanBuffer = buffer.CastAs<VulkanBuffer>();

	VmaAllocator memoryAllocator = vulkanDevice->GetMemoryAllocator();
	vk::PipelineStageFlagBits pipelineStage = vulkanBuffer->GetPipelineStage();

	if (buffer->GetIsGpuOnly())
	{
		VkBufferCreateInfo stagingCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };;
		stagingCreateInfo.size = size;
		stagingCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingAllocCreateInfo{};
		stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VkBuffer stagingBuf;
		VmaAllocation stagingAlloc;
		VmaAllocationInfo stagingAllocInfo;
		VK_CHECK_VOID(static_cast<vk::Result>(vmaCreateBuffer(memoryAllocator, &stagingCreateInfo, &stagingAllocCreateInfo, &stagingBuf, &stagingAlloc, &stagingAllocInfo)), "Failed to create staging buffer");

		VK_CHECK_VOID(static_cast<vk::Result>(vmaCopyMemoryToAllocation(memoryAllocator, data, stagingAlloc, 0, size)), "Failed to copy memory to staging buffer");
	}
	else
	{

	}
}

END_NAMESPACE_RHI