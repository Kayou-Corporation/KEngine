#include "Private/Vulkan/VulkanCommandList.hpp"

#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanDevice.hpp"
#include "Private/Vulkan/VulkanImage.hpp"

DISABLE_WARNINGS

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

void VulkanCommandList::SetBufferData(Core::RefCountPtr<Buffer> buffer, void* data, uint32_t size, uint32_t offset)
{
	ASSERT((size + offset) <= buffer->GetSize(), "Data is too large");

	Core::RefCountPtr<VulkanBuffer> vulkanBuffer = buffer.CastAs<VulkanBuffer>();

	VmaAllocator memoryAllocator = m_handle->memoryAllocator;

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

		TrackedStagingBufferPtr trackedStagingBuffer = Core::CreateRefPtr<TrackedStagingBuffer>();
		trackedStagingBuffer->handle = stagingBuf;
		trackedStagingBuffer->allocation = stagingAlloc;
		trackedStagingBuffer->allocationInfo = stagingAllocInfo;

		m_handle->trackedStagingBuffer = trackedStagingBuffer;
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

void VulkanCommandList::SetImageData(Core::RefCountPtr<Image> image, void* data, uint32_t size)
{
	Core::RefCountPtr<VulkanImage> vulkanImage = image.CastAs<VulkanImage>();

	if (vulkanImage->GetSource() == ImageSource::Gpu)
	{
		spdlog::error("Be carefull you tried to pass CPU data into a only GPU image");
		return;
	}

	VmaAllocator allocator = m_handle->memoryAllocator;

	// -------------------- STAGING BUFFER ----------------------- // 
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
	VK_CHECK_VOID(static_cast<vk::Result>(vmaCreateBuffer(allocator, &stagingCreateInfo, &stagingAllocCreateInfo, &stagingBuf, &stagingAlloc, &stagingAllocInfo)), "Failed to create staging buffer");

	VK_CHECK_VOID(static_cast<vk::Result>(vmaCopyMemoryToAllocation(allocator, data, stagingAlloc, 0, size)), "Failed to copy memory to staging buffer");


	// --------------------  COPY DATA TO GPU IMAGE ----------------------- // 
	vk::CommandBuffer cmdBuffer = m_handle->cmdBuffer;

	vk::BufferMemoryBarrier bufferMemBarrier{};
	bufferMemBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
	bufferMemBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
	bufferMemBarrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	bufferMemBarrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	bufferMemBarrier.buffer = stagingBuf;
	bufferMemBarrier.offset = 0;
	bufferMemBarrier.size = size;

	cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, bufferMemBarrier, nullptr);

	uint32_t layers = vulkanImage->GetLayersCount();
	uint32_t mips = vulkanImage->GetMipLevels();
	uint32_t bytesPerPixel = vulkanImage->GetBytesPerPixel();
	vk::Extent3D extent = vulkanImage->GetExtent();
	vk::ImageAspectFlags aspect = vulkanImage->GetAspect();

	std::vector<vk::BufferImageCopy> regions;
	vk::DeviceSize copyOffset = 0;
	
	for (uint32_t layer = 0; layer < layers; ++layer)
	{
		for (uint32_t mip = 0; mip < mips; ++mip)
		{
			vk::Extent3D mipExtent;
			mipExtent.width = std::max(1u, extent.width >> mip);
			mipExtent.height = std::max(1u, extent.height >> mip);
			mipExtent.depth = std::max(1u, extent.depth >> mip);

			vk::BufferImageCopy region;
			region.bufferOffset = copyOffset;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = aspect;
			region.imageSubresource.mipLevel = mip;
			region.imageSubresource.baseArrayLayer = layer;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = vk::Offset3D();
			region.imageExtent = mipExtent;

			regions.push_back(region);

			copyOffset += static_cast<vk::DeviceSize>(mipExtent.width) * mipExtent.height * mipExtent.depth * bytesPerPixel;
		}
	}

	cmdBuffer.copyBufferToImage(stagingBuf, vulkanImage->GetHandle(), vk::ImageLayout::eTransferDstOptimal, regions);


	// --------------------  GETTING RID OF THE STAGING BUFFER ----------------------- // 

	// Getting rid of the staging buffer later in Queue::Submit
	TrackedStagingBufferPtr trackedStagingBuffer = Core::CreateRefPtr<TrackedStagingBuffer>();
	trackedStagingBuffer->handle = stagingBuf;
	trackedStagingBuffer->allocation = stagingAlloc;
	trackedStagingBuffer->allocationInfo = stagingAllocInfo;
	m_handle->trackedStagingBuffer = trackedStagingBuffer;


	// --------------------  TRANSITION TO FINAL LAYOUT FOR USE ----------------------- // 
	vk::ImageLayout finalLayout = vulkanImage->GetLayout();


	// A little bit "hardcode" but this function should only be use with a final layout = transitionToFinalLayout
	vk::ImageMemoryBarrier transitionToFinalLayout{};
	transitionToFinalLayout.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	transitionToFinalLayout.newLayout = finalLayout;
	transitionToFinalLayout.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	transitionToFinalLayout.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	transitionToFinalLayout.image = vulkanImage->GetHandle();
	transitionToFinalLayout.subresourceRange.aspectMask = aspect;
	transitionToFinalLayout.subresourceRange.baseMipLevel = 0;
	transitionToFinalLayout.subresourceRange.levelCount = mips;
	transitionToFinalLayout.subresourceRange.baseArrayLayer = 0;
	transitionToFinalLayout.subresourceRange.layerCount = layers;
	transitionToFinalLayout.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	transitionToFinalLayout.dstAccessMask = vk::AccessFlagBits::eShaderRead;

	cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr, transitionToFinalLayout);
}


END_NAMESPACE_RHI