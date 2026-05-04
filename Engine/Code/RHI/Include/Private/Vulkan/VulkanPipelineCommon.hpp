#pragma once

#include "Public/PipelineCommon.hpp"

#include "VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanDescriptorSetLayout : public DescriptorSetLayout
{
public:
	VulkanDescriptorSetLayout() = default;
	virtual ~VulkanDescriptorSetLayout() override = default;

public:
	vk::DescriptorSetLayout GetHandle() { return m_descriptorSetLayout; }

	void SetHandle(vk::DescriptorSetLayout layout) { m_descriptorSetLayout = layout; }

private:
	vk::DescriptorSetLayout m_descriptorSetLayout;
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