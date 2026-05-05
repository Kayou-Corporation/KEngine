#pragma once

#include "Public/GraphicsPipeline.hpp"

#include "VulkanUtils.hpp"
#include "Private/Vulkan/VulkanShader.hpp"

BEGIN_NAMESPACE_RHI

struct VulkanGraphicsPipelineStructs
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

	// Do not manually set the members bellow

	vk::PipelineRenderingCreateInfo renderingInfo{}; // Do not manually set
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{}; // Do not manually set
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo{}; // Do not manually set
};

struct VulkanDescriptorSetLayoutSpecs
{
	std::vector<vk::DescriptorSetLayoutBinding> bindings{};
};

class VulkanGraphicsPipeline : public GraphicsPipeline
{
public:
	VulkanGraphicsPipeline() = default;
	virtual ~VulkanGraphicsPipeline() override = default;

public:
	VulkanGraphicsPipelineStructs GetGraphicsCreateInfo(const GraphicsPipelineSpecs& specs);
	vk::GraphicsPipelineCreateInfo GetVulkanGraphicsCreateInfo(VulkanGraphicsPipelineStructs& vulkanGraphicsPipelineSpecs);

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

	vk::GraphicsPipelineCreateInfo m_createInfo;
};

END_NAMESPACE_RHI