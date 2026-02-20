#pragma once

#include "Core/RHI/Private/Vulkan/VulkanUtils.hpp"
#include <vector>
#include <map>
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

	void Create(vk::Device& device);
	void Destroy(vk::Device& device);

	RefCountPtr<VulkanCommandList> GetOrCreateCommandBuffer(vk::Device& device);
	void Submit(RefCountPtr<VulkanCommandList> cmdList);

	void RunGarbageCollector(vk::Device& device);

	vk::Queue handle;
	uint32_t queueFamilyIndex;

	vk::Semaphore trackingSemaphore;
	//std::vector<vk::Semaphore> waitSemaprhores;
	//std::vector<vk::Semaphore> signalSemaprhores;

	std::list<TrackedCommandBufferPtr> commandBuffersPool;
	std::list<TrackedCommandBufferPtr> inFlightCommandBuffersPool;

	uint64_t lastSubmitdId = 0;
	uint64_t lastFinishedId = 0;

	//vk::QueueFlagBits type;
	//
	//vk::CommandPool cmdPool;
	//
	//std::vector<vk::CommandBuffer> idleCommandBuffers;
	//std::vector<vk::Fence> idleFences;
	//
	//std::vector<vk::CommandBuffer> inFlightCommandBuffers;
	//std::vector<vk::Fence> inFlightFences;
	//
	//int currentIndex = -1;
	//int queueIndex;
};