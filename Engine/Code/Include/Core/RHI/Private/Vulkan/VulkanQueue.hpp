#pragma once

#include "Core/RHI/Private/Vulkan/VulkanUtils.hpp"
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "Core/RHI/Public/RHI.hpp"
#include "Core/Utils/Memory.hpp"

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

class TrackedCommandBuffer : virtual public IResource
{
public:
	TrackedCommandBuffer() = default;
	virtual ~TrackedCommandBuffer() override = default;

	vk::CommandPool cmdPool;
	vk::CommandBuffer cmdBuffer;

	uint64_t submissionId;
};
typedef RefCountPtr<TrackedCommandBuffer> TrackedCommandBufferPtr;

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

	void RunGarbageCollector(vk::Device& device);

private:
	vk::Queue m_handle;
	vk::QueueFlagBits m_queueType;
	uint32_t m_queueFamilyIndex;
	

	vk::Semaphore m_trackingSemaphore;
	//std::vector<vk::Semaphore> waitSemaprhores;
	//std::vector<vk::Semaphore> signalSemaprhores;

	std::list<TrackedCommandBufferPtr> m_commandBuffersPool;
	std::list<TrackedCommandBufferPtr> m_inFlightCommandBuffersPool;

	uint64_t m_lastSubmitdId = 0;
	uint64_t m_lastFinishedId = 0;
};