#include "Private/Vulkan/VulkanCommandList.hpp"

#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanDevice.hpp"
#include "Private/Vulkan/VulkanImage.hpp"
#include "Private/Vulkan/VulkanRenderpass.hpp"
#include "Private/Vulkan/VulkanGraphicsPipeline.hpp"
#include"Private/Vulkan/VulkanPipelineCommon.hpp"
#include"Private/Vulkan/VulkanDescriptorSet.hpp"

BEGIN_NAMESPACE_RHI

//----------- Open / Close --------------//
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

//----------- Dynamic Rendering --------------//
void VulkanCommandList::BeginRendering(const RenderingInfo& RHIRenderingInfo)
{
	vk::RenderingInfo info{};
	info.renderArea.offset = TranslateToVulkan(RHIRenderingInfo.offset);
	info.renderArea.extent = TranslateToVulkan(RHIRenderingInfo.extent);
	info.layerCount = RHIRenderingInfo.layerCount;
	info.colorAttachmentCount = RHIRenderingInfo.colorAttachmentCount;

	std::vector<vk::RenderingAttachmentInfo> colorAttachments;
	for (uint32_t i = 0; i < RHIRenderingInfo.colorAttachments.size(); ++i)
	{
		vk::RenderingAttachmentInfo colorAttachment = VulkanRenderpass::GetRenderingAttachmentInfo(RHIRenderingInfo.colorAttachments[i]);
		colorAttachments.push_back(colorAttachment);
	}
	info.pColorAttachments = colorAttachments.data();

	vk::RenderingAttachmentInfo depthAttachment = VulkanRenderpass::GetRenderingAttachmentInfo(RHIRenderingInfo.depthAttachment);
	info.pDepthAttachment = &depthAttachment;

	m_handle->cmdBuffer.beginRendering(info);
}

void VulkanCommandList::EndRendering()
{
	m_handle->cmdBuffer.endRendering();
}

//----------- Bind --------------//
void VulkanCommandList::BindDescriptorSet(Core::RefCountPtr<PipelineLayout> RHIPipelineLayout, std::string layoutName, DescriptorSetHandle RHIDescriptorSet, PipelineBindPoint RHIBindPoint)
{
	Core::RefCountPtr<VulkanPipelineLayout> RHVulkanIPipelineLayout = RHIPipelineLayout.CastAs<VulkanPipelineLayout>();
	Core::RefCountPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = RHIDescriptorSet.CastAs<VulkanDescriptorSet>();

	vk::PipelineLayout pipelineLayout = RHVulkanIPipelineLayout->GetHandle();
	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::PipelineBindPoint bindPoint = TranslateToVulkan(RHIBindPoint);
	uint32_t layoutIndex = RHVulkanIPipelineLayout->GetDescriptorSetLayoutIndex(layoutName);

	m_handle->cmdBuffer.bindDescriptorSets(bindPoint, pipelineLayout, layoutIndex, 1,&descriptorSet, 0, nullptr);
}

void VulkanCommandList::BindDescriptorSet(Core::RefCountPtr<PipelineLayout> RHIPipelineLayout, uint32_t layoutIndex, DescriptorSetHandle RHIDescriptorSet, PipelineBindPoint RHIBindPoint)
{
	Core::RefCountPtr<VulkanPipelineLayout> RHVulkanIPipelineLayout = RHIPipelineLayout.CastAs<VulkanPipelineLayout>();
	Core::RefCountPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = RHIDescriptorSet.CastAs<VulkanDescriptorSet>();

	vk::PipelineLayout pipelineLayout = RHVulkanIPipelineLayout->GetHandle();
	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::PipelineBindPoint bindPoint = TranslateToVulkan(RHIBindPoint);

	m_handle->cmdBuffer.bindDescriptorSets(bindPoint, pipelineLayout, layoutIndex, 1, &descriptorSet, 0, nullptr);
}

void VulkanCommandList::BindVertexBuffer(BufferHandle RHIVertexBuffer, uint32_t offset)
{
	Core::RefCountPtr<VulkanBuffer> RHVulkanVertexBuffer = RHIVertexBuffer.CastAs<VulkanBuffer>();
	
	vk::Buffer buffer = RHVulkanVertexBuffer->GetHandle();
	vk::DeviceSize vkOffset = static_cast<vk::DeviceSize>(offset);

	m_handle->cmdBuffer.bindVertexBuffers(0, { buffer }, { vkOffset });
}

void VulkanCommandList::BindIndexBuffer(BufferHandle RHIIndexBuffer, uint32_t offset)
{
	Core::RefCountPtr<VulkanBuffer> RHVulkanIndexBuffer = RHIIndexBuffer.CastAs<VulkanBuffer>();

	vk::Buffer buffer = RHVulkanIndexBuffer->GetHandle();
	vk::DeviceSize vkOffset = static_cast<vk::DeviceSize>(offset);

	m_handle->cmdBuffer.bindIndexBuffer(buffer, vkOffset, vk::IndexType::eUint32);
}

//----------- Draw --------------//
void VulkanCommandList::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	vk::Viewport viewport{};
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	m_handle->cmdBuffer.setViewport(0, 1, &viewport);
}

void VulkanCommandList::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	vk::Rect2D scissor{};
	scissor.offset = vk::Offset2D{ static_cast<int32_t>(x), static_cast<int32_t>(y) };
	scissor.extent = vk::Extent2D{ width, height };

	m_handle->cmdBuffer.setScissor(0, 1, &scissor);
}

void VulkanCommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
	m_handle->cmdBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

//----------- Set Buffer / Image Data --------------//
void VulkanCommandList::SetBufferData(BufferHandle RHIBuffer, void* data, uint32_t size, uint32_t offset)
{
	ASSERT((size + offset) <= RHIBuffer->GetSize(), "Data is too large");

	Core::RefCountPtr<VulkanBuffer> RHIVulkanBuffer = RHIBuffer.CastAs<VulkanBuffer>();

	VmaAllocator memoryAllocator = m_handle->memoryAllocator;

	VmaAllocation bufferAllocation = RHIVulkanBuffer->GetAllocation();
	vk::PipelineStageFlagBits pipelineStage = RHIVulkanBuffer->GetPipelineStage();
	vk::Buffer bufferHandle = RHIVulkanBuffer->GetHandle();
	vk::AccessFlags bufferAccessFlags = GetAccessFlagsFromUsage(RHIVulkanBuffer->GetPrimaryUsage());

	vk::CommandBuffer cmdBuffer = m_handle->cmdBuffer;

	VkMemoryPropertyFlags memPropFlags;
	vmaGetAllocationMemoryProperties(memoryAllocator, bufferAllocation, &memPropFlags);

	if (memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		VK_CHECK_VOID(static_cast<vk::Result>(vmaCopyMemoryToAllocation(memoryAllocator, data, bufferAllocation, offset, size)), "Failed to copy memory to buffer");

		// TODO : Check, not sure which size to use ? 
		vk::BufferMemoryBarrier bufferMemBarrier = GetBufferMemoryBarrier(RHIVulkanBuffer, offset, size, vk::AccessFlagBits::eHostWrite, bufferAccessFlags);

		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, pipelineStage, {}, nullptr, bufferMemBarrier, nullptr);
	}
	else
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

		vk::BufferMemoryBarrier bufferMemBarrier = GetRawBufferMemoryBarrier(stagingBuf, 0, size, vk::AccessFlagBits::eHostWrite, vk::AccessFlagBits::eTransferRead);

		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, bufferMemBarrier, nullptr);

		vk::BufferCopy bufferCopy{};
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = offset;
		bufferCopy.size = size;

		cmdBuffer.copyBuffer(stagingBuf, bufferHandle, bufferCopy);

		vk::BufferMemoryBarrier bufferMemBarrier2 = GetRawBufferMemoryBarrier(bufferHandle, offset, size, vk::AccessFlagBits::eTransferWrite, bufferAccessFlags);

		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, pipelineStage, {}, nullptr, bufferMemBarrier2, nullptr);

		TrackedStagingBufferPtr trackedStagingBuffer = Core::CreateRefPtr<TrackedStagingBuffer>();
		trackedStagingBuffer->handle = stagingBuf;
		trackedStagingBuffer->allocation = stagingAlloc;
		trackedStagingBuffer->allocationInfo = stagingAllocInfo;

		m_handle->trackedStagingBuffers.push_back(trackedStagingBuffer);
	}
}

void VulkanCommandList::CopyBufferToBuffer(BufferHandle RHISrcBuffer, uint32_t srcOffset, BufferHandle RHIDstBuffer, uint32_t dstOffset, uint32_t size, bool returnSrcBufferToInitialStage, bool returnDstBufferToInitialStage)
{
	Core::RefCountPtr<VulkanBuffer> RHIVulkanSrcBuffer = RHISrcBuffer.CastAs<VulkanBuffer>();
	Core::RefCountPtr<VulkanBuffer> RHIVulkanDstBuffer = RHIDstBuffer.CastAs<VulkanBuffer>();

	vk::Buffer srcBuffer = RHIVulkanSrcBuffer->GetHandle();
	vk::Buffer dstBuffer = RHIVulkanDstBuffer->GetHandle();

	ASSERT((srcOffset + size) <= RHIVulkanSrcBuffer->GetSize(), "Src data is to large with the offset");
	ASSERT((dstOffset + size) <= RHIVulkanDstBuffer->GetSize(), "Dst data is to large with the offset");

	vk::CommandBuffer cmdBuffer = m_handle->cmdBuffer;

	// -------------------- WE NEED TO PREPARE SRC BUFFER SO DST BUFFER CAN READ IT ----------------------- //
	vk::BufferMemoryBarrier srcBufferPreparationBarrier = GetRawBufferMemoryBarrier(srcBuffer, srcOffset, size, RHIVulkanSrcBuffer->GetAccessMask(), vk::AccessFlagBits::eTransferRead);

	vk::PipelineStageFlags srcStageMask = srcBufferPreparationBarrier.srcAccessMask == vk::AccessFlagBits::eHostWrite ? vk::PipelineStageFlagBits::eHost : RHIVulkanSrcBuffer->GetPipelineStage();
	cmdBuffer.pipelineBarrier(srcStageMask, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, srcBufferPreparationBarrier, nullptr);

	// -------------------- WE NEED TO PREPARE DST BUFFER IT CAN READ SRC BUFFER ----------------------- //
	vk::BufferMemoryBarrier dstBufferPreparationBarrier = GetRawBufferMemoryBarrier(dstBuffer, dstOffset, size, RHIVulkanDstBuffer->GetAccessMask(), vk::AccessFlagBits::eTransferWrite);

	vk::PipelineStageFlags dstBufferSrcStageMask = dstBufferPreparationBarrier.srcAccessMask == vk::AccessFlagBits::eHostWrite ? vk::PipelineStageFlagBits::eHost : RHIVulkanDstBuffer->GetPipelineStage();
	cmdBuffer.pipelineBarrier(dstBufferSrcStageMask, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, dstBufferPreparationBarrier, nullptr);

	// -------------------- ACTUAL COPY ----------------------- //
	vk::BufferCopy bufferCopy{};
	bufferCopy.srcOffset = srcOffset;
	bufferCopy.dstOffset = dstOffset;
	bufferCopy.size = vk::DeviceSize(size);

	cmdBuffer.copyBuffer(srcBuffer, dstBuffer, bufferCopy);

	// -------------------- REVERT SRC BUFFER TO INITIAL ACCESS MASK ----------------------- //
	// TODO : probable issue with current srcAccessMask if returnSrcBufferToInitialStage = false, because not updtated ?
	if (returnSrcBufferToInitialStage)
	{
		vk::BufferMemoryBarrier srcBufferPostCopyBarrier = GetRawBufferMemoryBarrier(srcBuffer, srcOffset, size, vk::AccessFlagBits::eTransferRead, RHIVulkanSrcBuffer->GetAccessMask());

		vk::PipelineStageFlags srcPostCopySrcStageMask = vk::PipelineStageFlagBits::eTransfer;
		vk::PipelineStageFlags srcPostCopyDstStageMask = RHIVulkanSrcBuffer->GetPipelineStage();

		cmdBuffer.pipelineBarrier(srcPostCopySrcStageMask, srcPostCopyDstStageMask, {}, nullptr, srcBufferPostCopyBarrier, nullptr);
	}

	// -------------------- REVERT DST BUFFER TO INITIAL ACCESS MASK ----------------------- //
	if (returnDstBufferToInitialStage)
	{
		vk::BufferMemoryBarrier dstBufferPostCopyBarrier = GetRawBufferMemoryBarrier(dstBuffer, dstOffset, size, vk::AccessFlagBits::eTransferWrite, RHIVulkanDstBuffer->GetAccessMask());

		vk::PipelineStageFlags dstPostCopySrcStageMask = vk::PipelineStageFlagBits::eTransfer;
		vk::PipelineStageFlags dstPostCopyDstStageMask = RHIVulkanDstBuffer->GetPipelineStage();

		cmdBuffer.pipelineBarrier(dstPostCopySrcStageMask, dstPostCopyDstStageMask, {}, nullptr, dstBufferPostCopyBarrier, nullptr);
	}
}

void VulkanCommandList::SetImageData(Core::RefCountPtr<Image> RHIImage, void* data, uint32_t size)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIImage.CastAs<VulkanImage>();

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

	vk::BufferMemoryBarrier bufferMemBarrier = GetRawBufferMemoryBarrier(stagingBuf, 0, size, vk::AccessFlagBits::eHostWrite, vk::AccessFlagBits::eTransferRead);

	cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, bufferMemBarrier, nullptr);

	uint32_t layers = RHIVulkanImage->GetLayersCount();
	uint32_t mips = RHIVulkanImage->GetMipLevels();
	uint32_t bytesPerPixel = RHIVulkanImage->GetBytesPerPixel();
	vk::Extent3D extent = RHIVulkanImage->GetVulkanExtent();
	vk::ImageAspectFlags aspect = RHIVulkanImage->GetAspect();

	vk::ImageMemoryBarrier transitionToCopyLayout = GetRawImageMemoryBarrier(RHIVulkanImage->GetHandle(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, aspect, mips, layers, vk::AccessFlagBits::eNone, vk::AccessFlagBits::eTransferWrite);

	cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, nullptr, transitionToCopyLayout);

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

	cmdBuffer.copyBufferToImage(stagingBuf, RHIVulkanImage->GetHandle(), vk::ImageLayout::eTransferDstOptimal, regions);

	// --------------------  GETTING RID OF THE STAGING BUFFER ----------------------- // 

	// Getting rid of the staging buffer later in Queue::Submit
	TrackedStagingBufferPtr trackedStagingBuffer = Core::CreateRefPtr<TrackedStagingBuffer>();
	trackedStagingBuffer->handle = stagingBuf;
	trackedStagingBuffer->allocation = stagingAlloc;
	trackedStagingBuffer->allocationInfo = stagingAllocInfo;
	m_handle->trackedStagingBuffers.push_back(trackedStagingBuffer);


	// --------------------  TRANSITION TO FINAL LAYOUT FOR USE ----------------------- // 
	vk::ImageLayout finalLayout = RHIVulkanImage->GetVulkanTargetLayout();

	// A little bit "hardcode" but this function should only be use with a final layout = transitionToFinalLayout
	vk::ImageMemoryBarrier transitionToFinalLayout = GetRawImageMemoryBarrier(RHIVulkanImage->GetHandle(), vk::ImageLayout::eTransferDstOptimal, finalLayout, aspect, mips, layers, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead);

	cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr, transitionToFinalLayout);
}

void VulkanCommandList::CopyImageToImage(Core::RefCountPtr<Image> RHISrcImage, Extent3D srcOffset, Core::RefCountPtr<Image> RHIDstImage, Extent3D dstOffset, bool returnSrcImageToInitialStage, bool returnDstImageToInitialStage)
{
	Core::RefCountPtr<VulkanImage> VulkanSrcImage = RHISrcImage.CastAs<VulkanImage>();
	Core::RefCountPtr<VulkanImage> VulkanDstImage = RHIDstImage.CastAs<VulkanImage>();

	vk::Image srcHangle = VulkanSrcImage->GetHandle();
	vk::Image dstHangle = VulkanDstImage->GetHandle();
	
	vk::ImageLayout srcImageLayout = VulkanSrcImage->GetVulkanCurrentLayout();
	vk::AccessFlags srcCurrentAccessMask = GetSrcAccessMask(srcImageLayout);
	
	vk::ImageLayout dstImageLayout = VulkanDstImage->GetVulkanCurrentLayout();
	vk::AccessFlags dstCurrentAccessMask = GetSrcAccessMask(dstImageLayout);

	vk::Offset3D vkSrcOffset = vk::Offset3D(srcOffset.x, srcOffset.y, srcOffset.z);
	vk::Offset3D vkDstOffset = vk::Offset3D(dstOffset.x, dstOffset.y, dstOffset.z);
	
	vk::CommandBuffer cmdBuffer = m_handle->cmdBuffer;
	
	//----------- Prepae Src image for transfer --------------//
	vk::ImageMemoryBarrier srcPreparationBarrier = GetImageMemoryBarrier(VulkanSrcImage, vk::ImageLayout::eTransferSrcOptimal, srcCurrentAccessMask, vk::AccessFlagBits::eTransferRead);

	//----------- Prepae Dst image for transfer --------------//
	vk::ImageMemoryBarrier dstPreparationBarrier = GetImageMemoryBarrier(VulkanDstImage, vk::ImageLayout::eTransferDstOptimal, dstCurrentAccessMask, vk::AccessFlagBits::eTransferWrite);

	//----------- Apply Preparation --------------//
	cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, nullptr, { srcPreparationBarrier, dstPreparationBarrier });

	//----------- Actual Copy --------------//
	
	vk::ImageCopy copyRegion{};
	copyRegion.srcSubresource.aspectMask = VulkanSrcImage->GetAspect();
	copyRegion.srcSubresource.mipLevel = 0;
	copyRegion.srcSubresource.baseArrayLayer = 0;
	copyRegion.srcSubresource.layerCount = VulkanSrcImage->GetLayersCount();
	copyRegion.srcOffset = vkSrcOffset;
	copyRegion.dstSubresource.aspectMask = VulkanDstImage->GetAspect();
	copyRegion.srcSubresource.mipLevel = 0;
	copyRegion.srcSubresource.baseArrayLayer = 0;
	copyRegion.srcSubresource.layerCount = VulkanDstImage->GetLayersCount();
	copyRegion.srcOffset = vkDstOffset;

	cmdBuffer.copyImage(srcHangle, srcImageLayout, dstHangle, dstImageLayout, copyRegion);

	//----------- Prepare Src image to return to initial stage --------------//
	if (returnSrcImageToInitialStage)
	{
		vk::ImageMemoryBarrier srcRestoreBarrier = GetImageMemoryBarrier(VulkanSrcImage, VulkanSrcImage->GetVulkanTargetLayout(), vk::AccessFlagBits::eTransferRead, srcCurrentAccessMask);
		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllCommands, {}, nullptr, nullptr, srcRestoreBarrier);
	}

	//----------- Prepare Dst image to return to initial stage --------------//
	if (returnDstImageToInitialStage)
	{
		vk::ImageMemoryBarrier dstRestoreBarrier = GetImageMemoryBarrier(VulkanDstImage, VulkanDstImage->GetVulkanTargetLayout(), vk::AccessFlagBits::eTransferWrite, dstCurrentAccessMask);
		cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllCommands, {}, nullptr, nullptr, dstRestoreBarrier);
	}
}

//----------- Transition Image Layout --------------//
void VulkanCommandList::TransitionImageLayout(Core::RefCountPtr<Image> RHIImage, Layout RHIDstLayout)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIImage.CastAs<VulkanImage>();

	vk::ImageLayout oldLayout = RHIVulkanImage->GetVulkanCurrentLayout();
	vk::ImageLayout newLayout = TranslateToVulkan(RHIDstLayout);

	vk::ImageMemoryBarrier barrier{};
	barrier.oldLayout = RHIVulkanImage->GetVulkanCurrentLayout();
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = RHIVulkanImage->GetHandle();

	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
	}
	else 
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = RHIVulkanImage->GetMipLevels();
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = RHIVulkanImage->GetLayersCount();

	vk::PipelineStageFlags srcStage;
	vk::PipelineStageFlags dstStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal) 
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) 
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else if (oldLayout == vk::ImageLayout::eColorAttachmentOptimal && newLayout == vk::ImageLayout::ePresentSrcKHR)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		barrier.dstAccessMask = vk::AccessFlags();
		srcStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dstStage = vk::PipelineStageFlagBits::eBottomOfPipe;
	}

	m_handle->cmdBuffer.pipelineBarrier(srcStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, barrier);	
	
	RHIVulkanImage->SetCurrentLayout(newLayout);
}

//----------- Pipeline --------------//
void VulkanCommandList::BindPipeline(Core::RefCountPtr<Pipeline> RHIPipeline)
{
	PipelineType RHIType = RHIPipeline->GetType();
	vk::PipelineBindPoint bindPoint = TranslateToVulkan(RHIType);

	vk::Pipeline pipeline;
	if (RHIType == PipelineType::Graphics)
	{
		pipeline = RHIPipeline.CastAs<VulkanGraphicsPipeline>()->GetHandle();

	}

	m_handle->cmdBuffer.bindPipeline(bindPoint, pipeline);
}


//------------------- PUBLIC VULKAN ------------------//
vk::BufferMemoryBarrier VulkanCommandList::GetBufferMemoryBarrier(Core::RefCountPtr<VulkanBuffer> RHIVulkanBuffer, uint32_t offset, uint32_t size, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask)
{
	vk::BufferMemoryBarrier barrier{};
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.buffer = RHIVulkanBuffer->GetHandle();
	barrier.offset = offset;
	barrier.size = size;

	return barrier;
}

vk::BufferMemoryBarrier VulkanCommandList::GetRawBufferMemoryBarrier(vk::Buffer buffer, uint32_t offset, uint32_t size, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask)
{
	vk::BufferMemoryBarrier barrier{};
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.buffer = buffer;
	barrier.offset = offset;
	barrier.size = size;

	return barrier;
}

vk::ImageMemoryBarrier VulkanCommandList::GetImageMemoryBarrier(Core::RefCountPtr<VulkanImage> RHIVulkanImage, vk::ImageLayout dstLayout, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask)
{
	vk::ImageLayout currentLayout = RHIVulkanImage->GetVulkanCurrentLayout();

	vk::ImageMemoryBarrier barrier{};
	barrier.oldLayout = currentLayout;
	barrier.newLayout = dstLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = RHIVulkanImage->GetHandle();
	barrier.subresourceRange.aspectMask = RHIVulkanImage->GetAspect();
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = RHIVulkanImage->GetMipLevels();
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = RHIVulkanImage->GetLayersCount();
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;

	return barrier;
}

vk::ImageMemoryBarrier VulkanCommandList::GetRawImageMemoryBarrier(vk::Image image, vk::ImageLayout currentLayout, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspect, uint32_t mipsLevel, uint32_t layersCount, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask)
{
	vk::ImageMemoryBarrier barrier{};
	barrier.oldLayout = currentLayout;
	barrier.newLayout = dstLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = aspect;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipsLevel;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layersCount;
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;

	return barrier;
}

vk::AccessFlags VulkanCommandList::GetSrcAccessMask(vk::ImageLayout layout)
{
	switch (layout)
	{
	case vk::ImageLayout::eUndefined:
		return vk::AccessFlagBits::eNone;

	case vk::ImageLayout::eColorAttachmentOptimal:
		return vk::AccessFlagBits::eColorAttachmentWrite;

	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
	case vk::ImageLayout::eDepthAttachmentOptimal:
	case vk::ImageLayout::eStencilAttachmentOptimal:
		return vk::AccessFlagBits::eDepthStencilAttachmentWrite;

	case vk::ImageLayout::eShaderReadOnlyOptimal:
		return vk::AccessFlagBits::eShaderRead;

	case vk::ImageLayout::eTransferSrcOptimal:
		return vk::AccessFlagBits::eTransferRead;

	case vk::ImageLayout::eTransferDstOptimal:
		return vk::AccessFlagBits::eTransferWrite;

	case vk::ImageLayout::ePresentSrcKHR:
		return vk::AccessFlagBits::eNone;

	case vk::ImageLayout::eGeneral:
		return vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;

	default:
		return vk::AccessFlagBits::eNone;
	}
}

END_NAMESPACE_RHI