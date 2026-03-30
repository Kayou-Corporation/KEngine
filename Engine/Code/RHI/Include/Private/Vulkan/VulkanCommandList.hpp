#pragma once

#include "Public/CommandList.hpp"
#include "Public/RHI.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanQueue.hpp"

BEGIN_NAMESPACE_RHI

class VulkanCommandList : public CommandList
{
public:
	VulkanCommandList() = default;
	virtual ~VulkanCommandList() override = default;

	virtual void Open() override;
	virtual void Close() override;

	virtual void SetBufferData(Core::RefCountPtr<Buffer> buffer, void* data, uint32_t size, uint32_t offset) override;
	virtual void SetImageData(Core::RefCountPtr<Image> image, void* data, uint32_t size, uint32_t offset) override;

public:
	void SetHandle(TrackedCommandBufferPtr& cmdBuffer) { m_handle = cmdBuffer; }
	TrackedCommandBufferPtr GetHandle() { return m_handle; }
	//QueueType GetOwnerQueueType() { return m_OwnerQueueType; }

private:
	TrackedCommandBufferPtr m_handle;
	
	//QueueType m_OwnerQueueType;
};

END_NAMESPACE_RHI