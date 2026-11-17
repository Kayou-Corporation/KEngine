#pragma once

#include <vector>

#include "RHI/API/DescriptorSet.hpp"

#include "VulkanPipelineCommon.hpp"

BEGIN_NAMESPACE_CORE
class VulkanDescriptorSet : public DescriptorSet
{
public:
    VulkanDescriptorSet() = delete;
    VulkanDescriptorSet(std::string name) : DescriptorSet(name) {}
    ~VulkanDescriptorSet() override = default;

    void SetHanlde(vk::DescriptorSet descriptorSet) { m_handle = descriptorSet; }
    void SetBindings(std::vector<VulkanBinding>& bindings) { m_bindings = bindings; }

    vk::DescriptorSet GetHandle() { return m_handle; }
    std::vector<VulkanBinding> GetAllBindings() { return m_bindings; }

private:
    vk::DescriptorSet m_handle;

    std::vector<VulkanBinding> m_bindings;
};

END_NAMESPACE_CORE