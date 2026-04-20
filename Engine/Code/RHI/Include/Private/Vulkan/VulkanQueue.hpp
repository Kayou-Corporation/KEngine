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
	void Submit(TrackedCommandBufferPtr cmdBuffer, vk::PipelineStageFlags waitStages);

	void PushSignalSemaphore(const vk::Semaphore& semaphore, uint64_t value); //{ signalSemaprhores.push_back(semaphore); }
	void PushSignalSemaphores(const std::vector<vk::Semaphore>& semaphores, const std::vector<uint64_t>& values); // { signalSemaprhores.insert(signalSemaprhores.end(), semaphores.begin(), semaphores.end()); }

	void PushWaitSemaphore(const vk::Semaphore& semaphore, uint64_t value);// { waitSemaprhores.push_back(semaphore); }
	void PushWaitSemaphores(const std::vector<vk::Semaphore>& semaphores, const std::vector<uint64_t>& values);// { waitSemaprhores.insert(waitSemaprhores.end(), semaphores.begin(), semaphores.end()); }

	void RunGarbageCollector(vk::Device& device);

	void SetAllocator(VmaAllocator allocator) { m_memoryAllocator = allocator; }

private:
	vk::Queue m_handle;
	vk::QueueFlagBits m_queueType;
	uint32_t m_queueFamilyIndex;
	VmaAllocator m_memoryAllocator;

	vk::Semaphore m_trackingSemaphore;
	std::vector<vk::Semaphore> m_signalSemaprhores;
	std::vector<uint64_t> m_signalSemaphoresValues;
	std::vector<vk::Semaphore> m_waitSemaprhores;
	std::vector<uint64_t> m_waitSemaprhoresValues;

	std::list<TrackedCommandBufferPtr> m_commandBuffersPool;
	std::list<TrackedCommandBufferPtr> m_inFlightCommandBuffersPool;

	uint64_t m_lastSubmitdId = 0;
	uint64_t m_lastFinishedId = 0;
};

END_NAMESPACE_RHI