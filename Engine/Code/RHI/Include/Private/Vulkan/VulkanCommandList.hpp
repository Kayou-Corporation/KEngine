#pragma once

#include "Public/CommandList.hpp"
#include "Public/RHI.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanQueue.hpp"
#include "Private/Vulkan/VulkanRenderpass.hpp"

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


	//----------- Set Buffer / Image Data --------------//
	virtual void SetBufferData(Core::RefCountPtr<Buffer> buffer, void* data, uint32_t size, uint32_t offset) override;
	virtual void SetImageData(Core::RefCountPtr<Image> image, void* data, uint32_t size) override;


	//----------- Transition Image Layout --------------//
	virtual void TransitionImageLayout(Core::RefCountPtr<Image> image, Layout dstLayout) override;

	//----------- Pipeline --------------//
	virtual void BindGraphicsPipeline(Core::RefCountPtr<GraphicsPipeline> RHIPipeline) override;

// Public vulkan
public:
	void SetHandle(TrackedCommandBufferPtr& cmdBuffer) { m_handle = cmdBuffer; }
	TrackedCommandBufferPtr GetHandle() { return m_handle; }

	void SetOwnerQueueType(QueueType queueType) { m_OwnerQueueType = queueType; }
	QueueType GetOwnerQueueType() { return m_OwnerQueueType; }

// Private vulkan
private:
	TrackedCommandBufferPtr m_handle;
	
	QueueType m_OwnerQueueType;
};

END_NAMESPACE_RHI