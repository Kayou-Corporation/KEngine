#pragma once

#include "RHI/API/GraphicsPipeline.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"
#include "RHI/Vulkan/VulkanShader.hpp"

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
	vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
	std::vector<vk::DynamicState> dynamicStates{};
	vk::PipelineLayout layout{};
	vk::RenderPass renderPass{};

	// Do not manually set the members bellow

	vk::PipelineRenderingCreateInfo renderingInfo{}; // Do not manually set
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{}; // Do not manually set
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo{}; // Do not manually set
	vk::PipelineColorBlendAttachmentState colorBlend{};
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

	vk::VertexInputBindingDescription GetBindingDescriptor(const VertexBindingLayout& RHIBindingLayout);
	vk::VertexInputAttributeDescription GetAttributeDescriptor(const VertexAttributeLayout& RHiAttributeLayout);

	vk::Pipeline GetHandle() const { return m_handle; }

	void SetHandle(vk::Pipeline handle) { m_handle = handle; }

private:
	vk::Pipeline m_handle;
};

END_NAMESPACE_RHI