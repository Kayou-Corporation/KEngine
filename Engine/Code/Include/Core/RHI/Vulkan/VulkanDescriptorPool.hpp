#pragma once

#include "CoreModule.hpp"

#include "RHI/API/DescriptorPool.hpp"
#include <vulkan/vulkan.hpp>

BEGIN_NAMESPACE_CORE



class VulkanDescriptorPool : public DescriptorPool
{
public:
    VulkanDescriptorPool() = default;
    virtual ~VulkanDescriptorPool() = default;

    vk::DescriptorPoolCreateInfo GetPoolCreateInfo(std::map<vk::DescriptorType, uint32_t> descriptors, uint32_t maxDescriptorSets);

    void SetPool(vk::DescriptorPool pool) {m_pool = pool;}
    vk::DescriptorPool GetHandle() const { return m_pool; }

private:
    vk::DescriptorPool m_pool;
    std::vector<vk::DescriptorPoolSize> m_poolSizes;
};
END_NAMESPACE_CORE