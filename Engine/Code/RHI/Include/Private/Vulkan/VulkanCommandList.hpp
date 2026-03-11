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

	void Open() override;
	void Close() override;

public:
	void SetHandle(TrackedCommandBufferPtr& cmdBuffer) { m_handle = cmdBuffer; }
	TrackedCommandBufferPtr GetHandle() { return m_handle; }
	//QueueType GetOwnerQueueType() { return m_OwnerQueueType; }

private:
	TrackedCommandBufferPtr m_handle;
	
	//QueueType m_OwnerQueueType;
};

END_NAMESPACE_RHI