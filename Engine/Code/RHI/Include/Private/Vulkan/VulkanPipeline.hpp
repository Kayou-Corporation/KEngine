#pragma once

#include "Public/Pipeline.hpp"

#include "VulkanUtils.hpp"
#include "Private/Vulkan/VulkanShader.hpp"

BEGIN_NAMESPACE_RHI

class VulkanPipeline : public Pipeline
{
public:
	VulkanPipeline() = default;
	virtual ~VulkanPipeline() override = default;

public:
	vk::GraphicsPipelineCreateInfo GetGraphicsCreateInfo(const PipelineSpecs& specs);
	vk::ComputePipelineCreateInfo GetComputeCreateInfo(const PipelineSpecs& specs);

	vk::PipelineLayoutCreateInfo GetPipelineLayoutCreateInfo();
	std::vector<vk::DescriptorSetLayoutCreateInfo> GetDescriptorSetLayoutCreateInfo(std::vector<Descriptor>& RHIDescriptors);

	vk::VertexInputBindingDescription GetBindingDescriptor(const VertexBindingLayout& RHIBindingLayout);
	vk::VertexInputAttributeDescription GetAttributeDescriptor(const VertexAttributeLayout& RHiAttributeLayout);

	vk::Pipeline GetHandle() const { return m_handle; }
	vk::PipelineLayout Getlayout() const { return m_layout; }
	std::vector<vk::DescriptorSetLayout> GetDescriptors() const { return m_descriptors; }

	void SetHandle(vk::Pipeline handle) { m_handle = handle; }
	void SetLayout(vk::PipelineLayout layout) { m_layout = layout; }
	void AddDescriptor(vk::DescriptorSetLayout descriptor) { m_descriptors.push_back(descriptor); }

private:
	vk::Pipeline m_handle;
	vk::PipelineLayout m_layout;
	std::vector<vk::DescriptorSetLayout> m_descriptors;
};

END_NAMESPACE_RHI