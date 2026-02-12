#pragma once

#include "Core/RHI/Private/Vulkan/VulkanUtils.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "Core/RHI/Public/RHI.hpp"

struct QueueFamily
{
public:
	static QueueFamily FindQueueFamily(const vk::PhysicalDevice& physicalDevice, std::vector<Queue> queues, bool searchPresentQueue, const vk::SurfaceKHR& surface);
	bool IsComplete() const;

	const std::unordered_map<Queue, std::optional<uint32_t>>& GetQueues() const { return m_queues; }

private:
	std::vector<Queue> m_requestedQueues;
	std::unordered_map<Queue, std::optional<uint32_t>> m_queues;

	bool m_searchPresentQueue;
	std::optional<uint32_t> m_presentQueue;
};