#include "Private/Vulkan/VulkanSyncronisation.hpp"

BEGIN_NAMESPACE_RHI

vk::SemaphoreCreateInfo VulkanSemaphore::GetCreateInfo(const SemaphoreSpecs& specs)
{
	vk::SemaphoreCreateInfo createInfo{};

	if (specs.type == SemaphoreType::Timeline)
	{
		vk::SemaphoreTypeCreateInfo typeInfo(vk::SemaphoreType::eTimeline, specs.timelineValue);
		createInfo.pNext = &typeInfo;
	}

	return createInfo;
}

END_NAMESPACE_RHI