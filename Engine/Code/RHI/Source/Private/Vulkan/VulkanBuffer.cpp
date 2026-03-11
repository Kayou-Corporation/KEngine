#include "Private/Vulkan/VulkanBuffer.hpp"

BEGIN_NAMESPACE_RHI

vk::BufferCreateInfo VulkanBuffer::GetCreateInfo(const BufferSpecs& specs)
{
	vk::BufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.usage = TranslateToVulkan(specs.usages);
	bufferCreateInfo.size = specs.size;

	return bufferCreateInfo;
}

END_NAMESPACE_RHI