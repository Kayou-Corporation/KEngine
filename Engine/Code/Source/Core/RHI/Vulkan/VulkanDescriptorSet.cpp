#include "RHI/Vulkan/VulkanDescriptorSet.hpp"

#include "RHI/Vulkan/VulkanBuffer.hpp"
#include "RHI/Vulkan/VulkanImage.hpp"
#include "RHI/Vulkan/VulkanTranslate.hpp"

BEGIN_NAMESPACE_CORE

vk::DescriptorSetAllocateInfo VulkanDescriptorSet::GetAllocInfo(vk::DescriptorSetLayout layout)
{
	(void)layout;

	vk::DescriptorSetAllocateInfo alloInfo{};
	alloInfo.descriptorPool = m_pool;
	alloInfo.descriptorSetCount = 1;
	
	return alloInfo;
}

vk::DescriptorPoolCreateInfo VulkanDescriptorSet::GetPoolCreateInfo(std::map<vk::DescriptorType, uint32_t> descriptors, uint32_t totalCount)
{
	(void)totalCount;
	vk::DescriptorPoolCreateInfo createInfo{};

	for (const auto& [type, count] : descriptors)
	{
		vk::DescriptorPoolSize typeSize;
		typeSize.type = type;
		typeSize.descriptorCount = count;
		m_poolSizes.push_back(typeSize);
	}

	createInfo.poolSizeCount = m_poolSizes.size();
	createInfo.pPoolSizes = m_poolSizes.data();
	createInfo.maxSets = 1;

	return createInfo;
}

END_NAMESPACE_CORE