#pragma once

#include "RHI/API/DescriptorSetArray.hpp"
#include "VulkanPipelineCommon.hpp"

BEGIN_NAMESPACE_CORE

class VulkanDescriptorSetArray : public DescriptorSetArray
{
public:
	VulkanDescriptorSetArray() = delete;
	VulkanDescriptorSetArray(std::string name) : DescriptorSetArray(name) {}
	~VulkanDescriptorSetArray() = default;

	void SetHanlde(vk::DescriptorSet descriptorSet) { m_handle = descriptorSet; }
	void SetBindings(std::vector<VulkanBinding>& bindings) { m_bindings = bindings; }

	vk::DescriptorSet GetHandle() { return m_handle; }
	std::vector<VulkanBinding> GetAllBindings() { return m_bindings; }

private:
	vk::DescriptorSet m_handle;
	std::vector<VulkanBinding> m_bindings;
};

END_NAMESPACE_CORE