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

struct Queue
{
public:
	Queue() = default;
	~Queue() = default;

	RefCountPtr<VulkanCommandList> GetOrCreateCommandBuffer(vk::Device& device);
	void Submit(RefCountPtr<VulkanCommandList> cmdList);

	void RunGarbageCollector(vk::Device& device);

	vk::Queue handle;
	vk::QueueFlagBits type;

	vk::CommandPool cmdPool;
	std::vector<vk::CommandBuffer> idleCommandBuffers;
	std::vector<vk::Fence> idleFences;

	std::vector<vk::CommandBuffer> inFlightCommandBuffers;
	std::vector<vk::Fence> inFlightFences;

	int currentIndex = -1;
	int queueIndex;
};