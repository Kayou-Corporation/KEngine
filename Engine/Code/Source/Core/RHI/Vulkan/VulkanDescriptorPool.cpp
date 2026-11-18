#include "RHI/Vulkan/VulkanDescriptorPool.hpp"

BEGIN_NAMESPACE_CORE

vk::DescriptorPoolCreateInfo VulkanDescriptorPool::GetPoolCreateInfo(std::map<vk::DescriptorType, uint32_t> descriptors, uint32_t maxDescriptorSets, bool useForArray)
{
    vk::DescriptorPoolCreateInfo createInfo{};

    for (const auto& [type, count] : descriptors)
    {
        vk::DescriptorPoolSize typeSize;
        typeSize.type = type;
        typeSize.descriptorCount = count;
        m_poolSizes.push_back(typeSize);
    }

    createInfo.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
    createInfo.pPoolSizes = m_poolSizes.data();
    createInfo.maxSets = maxDescriptorSets;
    createInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

    if (useForArray)
    {
        createInfo.flags |= vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
    }

    return createInfo;
}

END_NAMESPACE_CORE
