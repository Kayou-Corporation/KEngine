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

	void SetHandle(TrackedCommandBufferPtr& cmdBuffer) { m_handle = cmdBuffer; }
	//void SetIndex(uint32_t index) { m_index = index; }
	//void SetOwnerQueueType(QueueType type) { m_OwnerQueueType = type; }

	TrackedCommandBufferPtr GetHandle() { return m_handle; }
	//uint32_t GetIndex() { return m_index; }
	QueueType GetOwnerQueueType() { return m_OwnerQueueType; }

private:
	TrackedCommandBufferPtr m_handle;
	
	QueueType m_OwnerQueueType;
};