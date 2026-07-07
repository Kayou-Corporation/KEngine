#include "RHI/Vulkan/VulkanBuffer.hpp"

BEGIN_NAMESPACE_CORE

vk::BufferCreateInfo VulkanBuffer::GetCreateInfo(const BufferSpecs& specs)
{
	vk::BufferUsageFlags primaryUsage = TranslateToVulkan(specs.primaryUsage);
	vk::BufferUsageFlags usage = primaryUsage | TranslateToVulkan(specs.additionalUsages);
	uint32_t size = specs.size;

	m_primaryUsage = primaryUsage;
	m_access = specs.memoryAccess;
	m_usage = usage;
	m_size = specs.size;
	m_pipelineStage = TranslateToVulkan(specs.pipelineStage);
	m_isPersistentMapped = specs.isPersistentMapped;

	vk::BufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.size = size;

	return bufferCreateInfo;
}

vk::AccessFlags VulkanBuffer::GetAccessMask()
{
    if (!m_isGpuOnly && GetMappedData() != nullptr)
    {
        return vk::AccessFlagBits::eHostWrite;
    }

    vk::AccessFlags writeFlags = GetWriteAccessFlagsFromUsage(m_primaryUsage);

    if (writeFlags)
    {
        return writeFlags;
    }

    return vk::AccessFlags{};
}

END_NAMESPACE_CORE