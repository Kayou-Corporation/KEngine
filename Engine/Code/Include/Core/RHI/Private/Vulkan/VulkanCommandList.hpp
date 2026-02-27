#pragma once

#include "Core/RHI/Public/CommandList.hpp"
#include "Core/RHI/Public/RHI.hpp"

#include "Core/RHI/Private/Vulkan/VulkanUtils.hpp"
#include "Core/RHI/Private/Vulkan/VulkanQueue.hpp"

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