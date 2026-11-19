#pragma once

#include "RHI/API/DescriptorSetArray.hpp"

#include <vulkan/vulkan.hpp>

BEGIN_NAMESPACE_CORE

class VulkanDescriptorSetArray : public DescriptorSetArray
{
public:
	VulkanDescriptorSetArray() = delete;
	VulkanDescriptorSetArray(std::string name) : m_name(name) {}
	~VulkanDescriptorSetArray() = default;

	void SetHanlde(vk::DescriptorSet descriptorSet) { m_handle = descriptorSet; }
	vk::DescriptorSet GetHandle() { return m_handle; }

private:
	vk::DescriptorSet m_handle;
};

END_NAMESPACE_CORE