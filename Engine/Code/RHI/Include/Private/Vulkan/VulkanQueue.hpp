#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "Public/RHI.hpp"
#include "Utils/Memory.hpp"
#include "Private/Vulkan/VulkanUtils.hpp"

DISABLE_WARNINGS

#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_RHI

class VulkanDevice;

struct QueueFamily
{
public:
	static QueueFamily FindQueueFamily(const vk::PhysicalDevice& physicalDevice, std::vector<QueueType> queues, bool searchPresentQueue, const vk::SurfaceKHR& surface);
	bool IsComplete() const;

	const std::unordered_map<QueueType, std::optional<uint32_t>>& GetQueues() const { return m_queues; }
	uint32_t GetPresentQueueIndex() { return m_presentQueue.value_or(0); }

private:
	std::vector<QueueType> m_requestedQueues;
	std::unordered_map<QueueType, std::optional<uint32_t>> m_queues;

	bool m_searchPresentQueue;
	std::optional<uint32_t> m_presentQueue;
};

class VulkanCommandList;

class TrackedStagingBuffer : virtual public Core::IResource
{
public:
	TrackedStagingBuffer() = default;
	virtual ~TrackedStagingBuffer() override = default;

	vk::Buffer handle;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;
};
typedef Core::RefCountPtr<TrackedStagingBuffer> TrackedStagingBufferPtr;

class TrackedCommandBuffer : virtual public Core::IResource
{
public:
	TrackedCommandBuffer() = default;
	virtual ~TrackedCommandBuffer() override = default;

	vk::CommandPool cmdPool;
	vk::CommandBuffer cmdBuffer;

	uint64_t submissionId;

	TrackedStagingBufferPtr trackedStagingBuffer;
	VmaAllocator memoryAllocator;
};
typedef Core::RefCountPtr<TrackedCommandBuffer> TrackedCommandBufferPtr;

struct Queue
{
public:
	Queue() = default;
	~Queue() = default;


	void Create(vk::Device& device, vk::Queue& queue, uint32_t index, vk::QueueFlagBits type);
	void Destroy(vk::Device& device);

	void WaitIdle();

	TrackedCommandBufferPtr GetOrCreateCommandBuffer(vk::Device& device);
	void Submit(TrackedCommandBufferPtr cmdBuffer);

	void RunGarbageCollector(Core::RefCountPtr<VulkanDevice>& device);

	void SetAllocator(VmaAllocator allocator) { m_memoryAllocator = allocator; }

private:
	vk::Queue m_handle;
	vk::QueueFlagBits m_queueType;
	uint32_t m_queueFamilyIndex;
	VmaAllocator m_memoryAllocator;

	vk::Semaphore m_trackingSemaphore;
	//std::vector<vk::Semaphore> waitSemaprhores;
	//std::vector<vk::Semaphore> signalSemaprhores;

	std::list<TrackedCommandBufferPtr> m_commandBuffersPool;
	std::list<TrackedCommandBufferPtr> m_inFlightCommandBuffersPool;

	uint64_t m_lastSubmitdId = 0;
	uint64_t m_lastFinishedId = 0;
};

END_NAMESPACE_RHI