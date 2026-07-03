#pragma once

#include "Public/CommandList.hpp"
#include "Public/RHI.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanQueue.hpp"
#include "Private/Vulkan/VulkanRenderpass.hpp"
#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanImage.hpp"

BEGIN_NAMESPACE_RHI

class VulkanCommandList : public CommandList
{
// Public
public:
	VulkanCommandList() = default;
	virtual ~VulkanCommandList() override = default;

	//----------- Open / Close --------------//
	virtual void Open() override;
	virtual void Close() override;


	//----------- Dynamic Rendering --------------//
	virtual void BeginRendering(const RenderingInfo& renderingInfo) override;
	virtual void EndRendering() override;

	//----------- Bind --------------//
	virtual void BindDescriptorSet(PipelineLayoutHandle pipelineLayout, std::string layoutName, DescriptorSetHandle DescriptorSet, PipelineBindPoint bindPoint) override;
	virtual void BindDescriptorSet(PipelineLayoutHandle pipelineLayout, uint32_t layoutIndex, DescriptorSetHandle descriptorSet, PipelineBindPoint bindPoint) override;
	virtual void BindVertexBuffer(BufferHandle vertexBuffer, uint32_t offset) override;
	virtual void BindIndexBuffer(BufferHandle indexBuffer, uint32_t offset) override;

	//----------- Draw --------------//
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) override;

	//----------- Set Buffer / Image Data --------------//
	virtual void SetBufferData(BufferHandle buffer, void* data, uint32_t size, uint32_t offset) override;
	virtual void CopyBufferToBuffer(BufferHandle srcBuffer, uint32_t srcOffset, BufferHandle dstBuffer, uint32_t dstOffset, uint32_t size, bool returnSrcBufferToInitialStage = true, bool returnDstBufferToInitialStage = true) override;
	virtual void SetImageData(Core::RefCountPtr<Image> image, void* data, uint32_t size) override;
	virtual void CopyImageToImage(Core::RefCountPtr<Image> srcImage, Extent3D srcOffset, Core::RefCountPtr<Image> dstImage, Extent3D dstOffset, bool returnSrcImageToInitialStage = true, bool returnDstImageToInitialStage = true) override;

	//----------- Transition Image Layout --------------//
	virtual void TransitionImageLayout(Core::RefCountPtr<Image> image, Layout dstLayout) override;

	//----------- Pipeline --------------//
	virtual void BindPipeline(PipelineHandle RHIPipeline) override;

// Public vulkan
public:
	void SetHandle(TrackedCommandBufferPtr& cmdBuffer) { m_handle = cmdBuffer; }
	TrackedCommandBufferPtr GetHandle() { return m_handle; }

	void SetOwnerQueueType(QueueType queueType) { m_OwnerQueueType = queueType; }
	QueueType GetOwnerQueueType() { return m_OwnerQueueType; }

	vk::BufferMemoryBarrier GetBufferMemoryBarrier(Core::RefCountPtr<VulkanBuffer> RHIVulkanBuffer, uint32_t offset, uint32_t size, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask);
	vk::BufferMemoryBarrier GetRawBufferMemoryBarrier(vk::Buffer buffer, uint32_t offset, uint32_t size, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask);

	vk::ImageMemoryBarrier GetImageMemoryBarrier(Core::RefCountPtr<VulkanImage> RHIVulkanImage, vk::ImageLayout dstLayout, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask);
	vk::ImageMemoryBarrier GetRawImageMemoryBarrier(vk::Image image, vk::ImageLayout currentLayout, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspect, uint32_t mipsLevel, uint32_t layersCount, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask);

	vk::AccessFlags GetSrcAccessMask(vk::ImageLayout layout);

// Private vulkan
private:
	TrackedCommandBufferPtr m_handle;
	
	QueueType m_OwnerQueueType;
};

END_NAMESPACE_RHI