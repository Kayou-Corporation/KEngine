#pragma once

#include "Public/DescriptorSet.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanPipelineCommon.hpp"

#include <map>

BEGIN_NAMESPACE_RHI

class VulkanDescriptorSet : public DescriptorSet
{
public:
	VulkanDescriptorSet() = default;
	virtual ~VulkanDescriptorSet() override = default;

public:
	vk::DescriptorSetAllocateInfo GetAllocInfo(vk::DescriptorSetLayout layout);
	vk::DescriptorPoolCreateInfo GetPoolCreateInfo(std::map<vk::DescriptorType, uint32_t> descriptors, uint32_t totalCount);

	void SetHanlde(vk::DescriptorSet descriptorSet) { m_handle = descriptorSet; }
	void SetPool(vk::DescriptorPool pool) { m_pool = pool; }
	void SetBindings(std::vector<VulkanBinding>& bindings) { m_bindings = bindings; }

	vk::DescriptorSet GetHandle() { return m_handle; }
	vk::DescriptorPool GetPool() { return m_pool; }
	std::vector<VulkanBinding> GetAllBindings() { return m_bindings; }

private:
	vk::DescriptorSet m_handle;
	vk::DescriptorPool m_pool;
	std::vector<vk::DescriptorPoolSize> m_poolSizes;

	std::vector<VulkanBinding> m_bindings;
};

END_NAMESPACE_RHI