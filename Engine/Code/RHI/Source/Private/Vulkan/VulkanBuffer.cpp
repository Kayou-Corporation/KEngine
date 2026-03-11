#include "Private/Vulkan/VulkanBuffer.hpp"

BEGIN_NAMESPACE_RHI

void VulkanBuffer::SetData(void* data, uint32_t size)
{
	ASSERT(m_isGpuOnly, "Buffer is GPU only");
	ASSERT(size > m_size, "Data is too large");

	memcpy(m_allocationInfo.pMappedData, data, static_cast<size_t>(size));
}

vk::BufferCreateInfo VulkanBuffer::GetCreateInfo(const BufferSpecs& specs)
{
	vk::BufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.usage = TranslateToVulkan(specs.usages);
	bufferCreateInfo.size = specs.size;

	return bufferCreateInfo;
}

END_NAMESPACE_RHI