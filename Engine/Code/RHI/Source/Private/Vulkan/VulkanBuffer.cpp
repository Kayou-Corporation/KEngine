#include "Private/Vulkan/VulkanBuffer.hpp"

BEGIN_NAMESPACE_RHI

vk::BufferCreateInfo VulkanBuffer::GetCreateInfo(const BufferSpecs& specs)
{
	vk::BufferUsageFlags primaryUsage = TranslateToVulkan(specs.primaryUsage);
	vk::BufferUsageFlags additionalUsages = TranslateToVulkan(specs.additionalUsages);
	uint32_t size = specs.size;

	m_primaryUsage = primaryUsage;
	m_size = specs.size;
	m_pipelineStage = TranslateToVulkan(specs.pipelineStage);

	vk::BufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.usage = primaryUsage | additionalUsages;
	bufferCreateInfo.size = size;

	return bufferCreateInfo;
}

END_NAMESPACE_RHI