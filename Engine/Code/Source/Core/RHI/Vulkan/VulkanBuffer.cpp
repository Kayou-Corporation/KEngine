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
	m_pipelineStage = TranslateToVulkan(specs.pipelineStage) | TranslateToVulkan(specs.additionalStages);
	m_isPersistentMapped = specs.isPersistentMapped;

	if (specs.primaryUsage == BufferUsage::Storage)
		m_isReadOnly = true;
	else
	{
		for (const BufferUsage& additionalUsage : specs.additionalUsages)
		{
			if (additionalUsage == BufferUsage::Storage)
			{
				m_isReadOnly = true;
				break;
			}
		}
	}

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

    vk::AccessFlags writeFlags = GetWriteAccessFlagsFromUsage(m_primaryUsage, m_isReadOnly);

    if (writeFlags)
    {
        return writeFlags;
    }

    return vk::AccessFlags{};
}

END_NAMESPACE_CORE