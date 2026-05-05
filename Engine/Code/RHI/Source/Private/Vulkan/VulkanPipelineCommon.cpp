#include "Private/Vulkan/VulkanPipelineCommon.hpp"

BEGIN_NAMESPACE_RHI

VulkanBinding VulkanDescriptorSetLayout::GetBinding(std::string name)
{
	for (const VulkanBinding& binding : m_bindings)
	{
		if (binding.name == name)
		{
			return binding;
		}
	}

	spdlog::warn("Binding {} doesn't exist in this descriptor set layout", name);
	return VulkanBinding();
}

VulkanBinding VulkanDescriptorSetLayout::GetBinding(uint32_t bindingIndex)
{
	for (const VulkanBinding& binding : m_bindings)
	{
		if (binding.binding.binding == bindingIndex)
		{
			return binding;
		}
	}

	spdlog::warn("Binding index {} doesn't exist in this descriptor set layout", bindingIndex);
	return VulkanBinding();
}

std::vector<std::string> VulkanDescriptorSetLayout::GetAllBindingsNames()
{
	std::vector<std::string> allDescriptorSetsLayoutsNames;
	for (const VulkanBinding& binding : m_bindings)
	{
		allDescriptorSetsLayoutsNames.push_back(binding.name);
	}

	return allDescriptorSetsLayoutsNames;
}

std::vector<vk::DescriptorSetLayoutBinding> VulkanDescriptorSetLayout::GetAllVulkanBindings()
{
	std::vector<vk::DescriptorSetLayoutBinding> bindings;

	for (const VulkanBinding& vulkanBinding : m_bindings)
	{
		bindings.push_back(vulkanBinding.binding);
	}

	return bindings;
}

END_NAMESPACE_RHI