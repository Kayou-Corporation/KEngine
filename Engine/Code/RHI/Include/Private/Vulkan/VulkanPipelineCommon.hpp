#pragma once

#include "Public/PipelineCommon.hpp"

#include "VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

struct VulkanBinding
{
	VulkanBinding() = default;
	~VulkanBinding() = default;

	std::string name;
	vk::DescriptorSetLayoutBinding binding;
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
	std::vector<vk::DescriptorSetLayoutBinding>& GetAllVulkanBindings();

	void SetHandle(vk::DescriptorSetLayout layout) { m_descriptorSetLayout = layout; }
	void AddBinding(VulkanBinding binding) { m_bindings.push_back(binding); }

private:
	vk::DescriptorSetLayout m_descriptorSetLayout;
	std::vector<VulkanBinding> m_bindings;
	std::vector<vk::DescriptorSetLayoutBinding> m_vulkanBindings;
};

struct VulkanConstant
{
	VulkanConstant() = default;
	~VulkanConstant() = default;

	std::string constantName;
	uint32_t offset;
	uint32_t size;
};

class VulkanPushConstantLayout : public PushConstantLayout
{
public:
	VulkanPushConstantLayout() = default;
	virtual ~VulkanPushConstantLayout() override = default;

public:
	vk::PushConstantRange GetHandle() { return m_pushConstantRange; }
	std::vector<VulkanConstant> GetAllConstants() { return m_constants; }
	VulkanConstant GetConstant(std::string name);

	void SetHandle(vk::PushConstantRange range) { m_pushConstantRange = range; }
	void AddConstant(VulkanConstant constant) { m_constants.push_back(constant); }

private:
	std::vector<VulkanConstant> m_constants;
	vk::PushConstantRange m_pushConstantRange;
};

class VulkanPipelineLayout : public PipelineLayout
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