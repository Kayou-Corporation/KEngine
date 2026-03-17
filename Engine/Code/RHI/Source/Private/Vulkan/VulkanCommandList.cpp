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

	Core::RefCountPtr<VulkanDevice> vulkanDevice = device.CastAs<VulkanDevice>();
	Core::RefCountPtr<VulkanBuffer> vulkanBuffer = buffer.CastAs<VulkanBuffer>();

	VmaAllocator memoryAllocator = vulkanDevice->GetMemoryAllocator();

	VmaAllocation bufferAllocation = vulkanBuffer->GetAllocation();
	vk::PipelineStageFlagBits pipelineStage = vulkanBuffer->GetPipelineStage();
	vk::Buffer bufferHandle = vulkanBuffer->GetHandle();
	vk::AccessFlags bufferAccessFlags = GetAccessFlagsFromUsage(vulkanBuffer->GetUsage());

	vk::CommandBuffer cmdBuffer = m_handle->cmdBuffer;

	if (buffer->GetIsGpuOnly())
	{
#pragma region C-Style VMA
		VkBufferCreateInfo stagingCreateInfo{};
		stagingCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
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
#pragma endregion

		vk::BufferMemoryBarrier bufferMemBarrier{};
		bufferMemBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		bufferMemBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
		bufferMemBarrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		bufferMemBarrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		bufferMemBarrier.buffer = stagingBuf;
		bufferMemBarrier.offset = 0;
		bufferMemBarrier.size = size;

		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, bufferMemBarrier, nullptr);

		vk::BufferCopy bufferCopy{};
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = offset;
		bufferCopy.size = size;

		cmdBuffer.copyBuffer(stagingBuf, bufferHandle, bufferCopy);

		vk::BufferMemoryBarrier bufferMemBarrier2{};
		bufferMemBarrier2.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		bufferMemBarrier2.dstAccessMask = bufferAccessFlags;
		bufferMemBarrier2.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		bufferMemBarrier2.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		bufferMemBarrier2.buffer = bufferHandle;
		bufferMemBarrier2.offset = offset;
		bufferMemBarrier2.size = size;

		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, pipelineStage, {}, nullptr, bufferMemBarrier2, nullptr);

		// TODO : manage staging buffer destruction
	}
	else
	{
		VK_CHECK_VOID(static_cast<vk::Result>(vmaCopyMemoryToAllocation(memoryAllocator, data, bufferAllocation, offset, size)), "Failed to copy memory to buffer");

		vk::BufferMemoryBarrier bufferMemBarrier{};
		bufferMemBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		bufferMemBarrier.dstAccessMask = bufferAccessFlags;
		bufferMemBarrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		bufferMemBarrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		bufferMemBarrier.buffer = bufferHandle;
		bufferMemBarrier.offset = offset;
		bufferMemBarrier.size = size;

		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, pipelineStage, {}, nullptr, bufferMemBarrier, nullptr);
	}
}

END_NAMESPACE_RHI