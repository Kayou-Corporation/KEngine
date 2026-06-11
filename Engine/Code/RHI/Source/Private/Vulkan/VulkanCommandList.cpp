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
void VulkanCommandList::BindDescriptorSet(Core::RefCountPtr<PipelineLayout> RHIPipelineLayout, std::string layoutName, Core::RefCountPtr<DescriptorSet> RHIDescriptorSet, PipelineBindPoint RHIBindPoint)
{
	Core::RefCountPtr<VulkanPipelineLayout> RHVulkanIPipelineLayout = RHIPipelineLayout.CastAs<VulkanPipelineLayout>();
	Core::RefCountPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = RHIDescriptorSet.CastAs<VulkanDescriptorSet>();

	vk::PipelineLayout pipelineLayout = RHVulkanIPipelineLayout->GetHandle();
	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::PipelineBindPoint bindPoint = TranslateToVulkan(RHIBindPoint);
	uint32_t layoutIndex = RHVulkanIPipelineLayout->GetDescriptorSetLayoutIndex(layoutName);

	m_handle->cmdBuffer.bindDescriptorSets(bindPoint, pipelineLayout, layoutIndex, 1,&descriptorSet, 0, nullptr);
}

void VulkanCommandList::BindDescriptorSet(Core::RefCountPtr<PipelineLayout> RHIPipelineLayout, uint32_t layoutIndex, Core::RefCountPtr<DescriptorSet> RHIDescriptorSet, PipelineBindPoint RHIBindPoint)
{
	Core::RefCountPtr<VulkanPipelineLayout> RHVulkanIPipelineLayout = RHIPipelineLayout.CastAs<VulkanPipelineLayout>();
	Core::RefCountPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = RHIDescriptorSet.CastAs<VulkanDescriptorSet>();

	vk::PipelineLayout pipelineLayout = RHVulkanIPipelineLayout->GetHandle();
	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::PipelineBindPoint bindPoint = TranslateToVulkan(RHIBindPoint);

	m_handle->cmdBuffer.bindDescriptorSets(bindPoint, pipelineLayout, layoutIndex, 1, &descriptorSet, 0, nullptr);
}

void VulkanCommandList::BindVertexBuffer(Core::RefCountPtr<Buffer> RHIVertexBuffer, uint32_t offset)
{
	Core::RefCountPtr<VulkanBuffer> RHVulkanVertexBuffer = RHIVertexBuffer.CastAs<VulkanBuffer>();
	
	vk::Buffer buffer = RHVulkanVertexBuffer->GetHandle();
	vk::DeviceSize vkOffset = static_cast<vk::DeviceSize>(offset);

	m_handle->cmdBuffer.bindVertexBuffers(0, { buffer }, { vkOffset });
}

void VulkanCommandList::BindIndexBuffer(Core::RefCountPtr<Buffer> RHIIndexBuffer, uint32_t offset)
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
void VulkanCommandList::SetBufferData(Core::RefCountPtr<Buffer> RHIBuffer, void* data, uint32_t size, uint32_t offset)
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

		m_handle->trackedStagingBuffers.push_back(trackedStagingBuffer);
	}
}

void VulkanCommandList::SetImageData(Core::RefCountPtr<Image> RHIImage, void* data, uint32_t size)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIImage.CastAs<VulkanImage>();

	if (RHIVulkanImage->GetSource() == ImageSource::Gpu)
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

	uint32_t layers = RHIVulkanImage->GetLayersCount();
	uint32_t mips = RHIVulkanImage->GetMipLevels();
	uint32_t bytesPerPixel = RHIVulkanImage->GetBytesPerPixel();
	vk::Extent3D extent = RHIVulkanImage->GetExtent();
	vk::ImageAspectFlags aspect = RHIVulkanImage->GetAspect();

	vk::ImageMemoryBarrier transitionToCopyLayout{};
	transitionToCopyLayout.oldLayout = vk::ImageLayout::eUndefined;
	transitionToCopyLayout.newLayout = vk::ImageLayout::eTransferDstOptimal;
	transitionToCopyLayout.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	transitionToCopyLayout.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	transitionToCopyLayout.image = RHIVulkanImage->GetHandle();
	transitionToCopyLayout.subresourceRange.aspectMask = aspect;
	transitionToCopyLayout.subresourceRange.baseMipLevel = 0;
	transitionToCopyLayout.subresourceRange.levelCount = mips;
	transitionToCopyLayout.subresourceRange.baseArrayLayer = 0;
	transitionToCopyLayout.subresourceRange.layerCount = layers;
	transitionToCopyLayout.srcAccessMask = vk::AccessFlagBits::eNone;
	transitionToCopyLayout.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

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
	vk::ImageLayout finalLayout = RHIVulkanImage->GetFinalLayout();

	// A little bit "hardcode" but this function should only be use with a final layout = transitionToFinalLayout
	vk::ImageMemoryBarrier transitionToFinalLayout{};
	transitionToFinalLayout.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	transitionToFinalLayout.newLayout = finalLayout;
	transitionToFinalLayout.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	transitionToFinalLayout.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	transitionToFinalLayout.image = RHIVulkanImage->GetHandle();
	transitionToFinalLayout.subresourceRange.aspectMask = aspect;
	transitionToFinalLayout.subresourceRange.baseMipLevel = 0;
	transitionToFinalLayout.subresourceRange.levelCount = mips;
	transitionToFinalLayout.subresourceRange.baseArrayLayer = 0;
	transitionToFinalLayout.subresourceRange.layerCount = layers;
	transitionToFinalLayout.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	transitionToFinalLayout.dstAccessMask = vk::AccessFlagBits::eShaderRead;

	cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr, transitionToFinalLayout);
}

//----------- Transition Image Layout --------------//
void VulkanCommandList::TransitionImageLayout(Core::RefCountPtr<Image> RHIImage, Layout RHIDstLayout)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIImage.CastAs<VulkanImage>();

	vk::ImageLayout oldLayout = RHIVulkanImage->GetLayout();
	vk::ImageLayout newLayout = TranslateToVulkan(RHIDstLayout);

	vk::ImageMemoryBarrier barrier{};
	barrier.oldLayout = RHIVulkanImage->GetLayout();
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
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

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
	
	RHIVulkanImage->SetLayout(newLayout);
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

END_NAMESPACE_RHI