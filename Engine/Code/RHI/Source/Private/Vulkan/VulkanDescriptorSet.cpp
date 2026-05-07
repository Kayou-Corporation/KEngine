#include "Private/Vulkan/VulkanDescriptorSet.hpp"

#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanImage.hpp"
#include "Private/Vulkan/VulkanTranslate.hpp"

BEGIN_NAMESPACE_RHI

vk::DescriptorSetAllocateInfo VulkanDescriptorSet::GetAllocInfo(vk::DescriptorSetLayout layout)
{
	vk::DescriptorSetAllocateInfo alloInfo{};
	alloInfo.descriptorPool = m_pool;
	alloInfo.descriptorSetCount = 1;
	alloInfo.pSetLayouts = &layout;
	
	return alloInfo;
}

vk::DescriptorPoolCreateInfo VulkanDescriptorSet::GetPoolCreateInfo(std::map<vk::DescriptorType, uint32_t> descriptors, uint32_t totalCount)
{
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
	createInfo.maxSets = totalCount;

	return createInfo;
}

END_NAMESPACE_RHI