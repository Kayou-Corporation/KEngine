#pragma once

#include "Public/Pipeline.hpp"

#include "VulkanUtils.hpp"
#include "Private/Vulkan/VulkanShader.hpp"

BEGIN_NAMESPACE_RHI

struct VulkanGraphicsPipelineSpecs
{
	uint32_t colorAttachmentCount = 0;
	std::vector<vk::Format> colorAttachmentFormats{};
	vk::Format depthAttachment{};
	std::vector<vk::PipelineShaderStageCreateInfo> stages{};
	std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions{};
	std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions{};
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
	vk::PipelineViewportStateCreateInfo viewportState{};
	vk::PipelineRasterizationStateCreateInfo rasterizationState{};
	vk::PipelineMultisampleStateCreateInfo multisampleState{};
	vk::PipelineColorBlendStateCreateInfo colorBlendState{};
	std::vector<vk::DynamicState> dynamicStates{};
	vk::PipelineLayout layout{};
	vk::RenderPass renderPass{};

	// Do not manually set the members below

	vk::PipelineRenderingCreateInfo renderingInfo{}; // Do not manually set
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{}; // Do not manually set
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo{}; // Do not manually set
};

struct VulkanDescriptorSetLayoutSpecs
{
	std::vector<vk::DescriptorSetLayoutBinding> bindings{};
};

class VulkanPipeline : public Pipeline
{
public:
	VulkanPipeline() = default;
	virtual ~VulkanPipeline() override = default;

public:
	VulkanGraphicsPipelineSpecs GetGraphicsCreateInfo(const PipelineSpecs& specs);
	vk::GraphicsPipelineCreateInfo GetVulkanGraphicsCreateInfo(VulkanGraphicsPipelineSpecs& vulkanGraphicsPipelineSpecs);
	vk::ComputePipelineCreateInfo GetComputeCreateInfo(const PipelineSpecs& specs);

	vk::PipelineLayoutCreateInfo GetPipelineLayoutCreateInfo();
	std::vector<VulkanDescriptorSetLayoutSpecs> GetDescriptorSetLayoutCreateInfo(std::vector<Descriptor>& RHIDescriptors);
	std::vector<vk::DescriptorSetLayoutCreateInfo> GetVulkanDescriptorSetLayoutCreateInfo(std::vector<VulkanDescriptorSetLayoutSpecs>& RHIVulkanDescriptorSetLayoutSpecs);

	vk::VertexInputBindingDescription GetBindingDescriptor(const VertexBindingLayout& RHIBindingLayout);
	vk::VertexInputAttributeDescription GetAttributeDescriptor(const VertexAttributeLayout& RHiAttributeLayout);

	vk::Pipeline GetHandle() const { return m_handle; }
	vk::PipelineLayout GetLayout() const { return m_layout; }
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