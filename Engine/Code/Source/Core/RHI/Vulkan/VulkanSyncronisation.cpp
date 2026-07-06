#include "Private/Vulkan/VulkanSyncronisation.hpp"

BEGIN_NAMESPACE_RHI

vk::SemaphoreCreateInfo VulkanSemaphore::GetCreateInfo(const SemaphoreSpecs& specs)
{
	vk::SemaphoreCreateInfo createInfo{};

	vk::SemaphoreTypeCreateInfo typeInfo{};

	if (specs.type == SemaphoreType::Timeline)
	{
		typeInfo.semaphoreType = vk::SemaphoreType::eTimeline;
		typeInfo.initialValue = specs.timelineValue;
		createInfo.pNext = &typeInfo;
	}

	return createInfo;
}

END_NAMESPACE_RHI