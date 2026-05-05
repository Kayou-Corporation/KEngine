#pragma once

#include "Public/PipelineCommon.hpp"

#include "VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

struct VulkanBinding
{
	VulkanBinding() = default;
	~VulkanBinding() = default;

	std::string name;
	uint32_t setIndex;
	uint32_t bindingIndex;
	vk::ShaderStageFlagBits stage;
	vk::DescriptorType type;
	uint32_t count;
};

class VulkanDescriptorSetLayout : public DescriptorSetLayout
{
public:
	VulkanDescriptorSetLayout() = default;
	virtual ~VulkanDescriptorSetLayout() override = default;

public:
	vk::DescriptorSetLayout GetHandle() { return m_descriptorSetLayout; }
	VulkanBinding GetBinding(std::string name);
	VulkanBinding GetBinding(uint32_t bindingIndex);
	std::vector<std::string> GetAllBindingsNames();
	std::vector<VulkanBinding> GetAllBindings() { return m_bindings; }

	void SetHandle(vk::DescriptorSetLayout layout) { m_descriptorSetLayout = layout; }
	void AddBinding(VulkanBinding binding) { m_bindings.push_back(binding); }

private:
	vk::DescriptorSetLayout m_descriptorSetLayout;
	std::vector<VulkanBinding> m_bindings;
};

class VulkanPushConstantLayout : public PushConstantLayout
{
public:
	VulkanPushConstantLayout() = default;
	virtual ~VulkanPushConstantLayout() override = default;

public:
	vk::PushConstantRange GetHandle() { return m_pushConstantRange; }

	void SetHandle(vk::PushConstantRange layout) { m_pushConstantRange = layout; }

private:
	vk::PushConstantRange m_pushConstantRange;
};

class VulkanPipelineLayout : virtual PipelineLayout
{
public:
	VulkanPipelineLayout() = default;
	virtual ~VulkanPipelineLayout() override = default;

public:
	vk::PipelineLayout GetHandle() { return m_layout; }

	void SetHandle(vk::PipelineLayout layout) { m_layout = layout; }

private:
	vk::PipelineLayout m_layout;
};

END_NAMESPACE_RHI