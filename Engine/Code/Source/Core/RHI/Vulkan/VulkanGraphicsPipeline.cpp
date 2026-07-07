#include "RHI/Vulkan/VulkanGraphicsPipeline.hpp"
#include "RHI/Vulkan/VulkanPipelineCommon.hpp"

#include "RHI/Vulkan/VulkanTranslate.hpp"

BEGIN_NAMESPACE_RHI

VulkanGraphicsPipelineStructs VulkanGraphicsPipeline::GetGraphicsCreateInfo(const GraphicsPipelineSpecs& specs)
{
	VulkanGraphicsPipelineStructs createInfo{};

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos{};
	shaderStageInfos.reserve(specs.shaders.size());

	std::vector<VertexBindingLayout> RHIShadersBindings;
	std::vector<VertexAttributeLayout> RHIShadersAttributes;
	for (const auto& RHIShader : specs.shaders)
	{
		Core::RefCountPtr<VulkanShader> RHIVulkanShader = RHIShader.CastAs<VulkanShader>();
		ShaderStage stage = RHIShader->GetShaderStage();

		vk::PipelineShaderStageCreateInfo shaderStageCreateInfo{};
		shaderStageCreateInfo.stage = TranslateToVulkan(stage);
		shaderStageCreateInfo.module = RHIVulkanShader->GetModule();
		shaderStageCreateInfo.pName = "main";

		shaderStageInfos.push_back(shaderStageCreateInfo);

		std::vector<VertexBindingLayout> localRHIShaderBindings = RHIVulkanShader->GetVertexBindings();
		RHIShadersBindings.insert(RHIShadersBindings.end(), localRHIShaderBindings.begin(), localRHIShaderBindings.end());
		
		std::vector<VertexAttributeLayout> localRHIShaderAttributes = RHIVulkanShader->GetVertexAttributes();
		RHIShadersAttributes.insert(RHIShadersAttributes.end(), localRHIShaderAttributes.begin(), localRHIShaderAttributes.end());
	}

	// All unique Binding
	std::sort(RHIShadersBindings.begin(), RHIShadersBindings.end());
	RHIShadersBindings.erase(std::unique(RHIShadersBindings.begin(), RHIShadersBindings.end()), RHIShadersBindings.end());
	
	// All unique attributes
	std::sort(RHIShadersAttributes.begin(), RHIShadersAttributes.end());
	RHIShadersAttributes.erase(std::unique(RHIShadersAttributes.begin(), RHIShadersAttributes.end()), RHIShadersAttributes.end());

	// group attribute with respective binding group
	std::vector<std::pair<VertexBindingLayout, std::vector<VertexAttributeLayout>>> RHIBindingGroups;
	for (uint32_t i = 0; i < RHIShadersBindings.size(); ++i)
	{
		std::pair<VertexBindingLayout, std::vector<VertexAttributeLayout>> RHIBindingGroup = std::make_pair(RHIShadersBindings[i], std::vector<VertexAttributeLayout>());
		for (uint32_t j = 0; j < RHIShadersAttributes.size(); ++j)
		{
			if (RHIShadersAttributes[j].binding == RHIShadersBindings[i].binding)
			{
				RHIBindingGroup.second.push_back(RHIShadersAttributes[j]);
			}
		}

		RHIBindingGroups.push_back(RHIBindingGroup);
	}

	std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
	std::vector<vk::VertexInputAttributeDescription> AttributeDescriptions;
	for (uint32_t i = 0; i < RHIBindingGroups.size(); ++i)
	{
		bindingDescriptions.push_back(GetBindingDescriptor(RHIBindingGroups[i].first));
		for (uint32_t j = 0; j < RHIBindingGroups[i].second.size(); ++j)
		{
			AttributeDescriptions.push_back(GetAttributeDescriptor(RHIBindingGroups[i].second[j]));
		}
	}

	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	inputAssemblyInfo.topology = TranslateToVulkan(specs.topology);

	vk::PipelineViewportStateCreateInfo viewportStateInfo{};
	viewportStateInfo.viewportCount = specs.viewportCount;
	viewportStateInfo.scissorCount = specs.scissorCount;

	vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
	rasterizationInfo.lineWidth = specs.lineWidth;
	rasterizationInfo.cullMode = TranslateToVulkan(specs.cullmode);
	rasterizationInfo.frontFace = TranslateToVulkan(specs.frontFace);

	vk::PipelineMultisampleStateCreateInfo multisamplingInfo{};
	multisamplingInfo.rasterizationSamples = TranslateToVulkan(specs.SamplesCount);


	vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
	if (specs.blendColor)
	{
		colorBlendAttachment.blendEnable = VK_TRUE;
	}
	vk::PipelineColorBlendStateCreateInfo colorBlendingInfo{};
	colorBlendingInfo.attachmentCount = 1;
	colorBlendingInfo.pAttachments = &colorBlendAttachment;

	vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
	depthStencilCreateInfo.depthTestEnable = specs.depthTest;
	depthStencilCreateInfo.depthWriteEnable = specs.depthWrite;
	depthStencilCreateInfo.depthCompareOp = TranslateToVulkan(specs.depthCompare); 
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;


	std::vector<vk::DynamicState> dynamicStates = TranslateToVulkan(specs.dynamicStates);
	std::vector<vk::Format> colorAttachmentFormats = TranslateToVulkan(specs.colorAttachmentFormats);

	createInfo.colorAttachmentCount = specs.colorAttachmentCount;
	createInfo.colorAttachmentFormats = colorAttachmentFormats;
	createInfo.depthAttachment = TranslateToVulkan(specs.depthAttachment);
	createInfo.stages = shaderStageInfos;
	createInfo.vertexInputBindingDescriptions = bindingDescriptions;
	createInfo.vertexInputAttributeDescriptions = AttributeDescriptions;
	createInfo.inputAssemblyState = inputAssemblyInfo;
	createInfo.viewportState = viewportStateInfo;
	createInfo.rasterizationState = rasterizationInfo;
	createInfo.multisampleState = multisamplingInfo;
	createInfo.colorBlendState = colorBlendingInfo;
	createInfo.depthStencilCreateInfo = depthStencilCreateInfo;
	createInfo.dynamicStates = dynamicStates;
	createInfo.layout = m_layout.CastAs<VulkanPipelineLayout>()->GetHandle();
	createInfo.colorBlend = colorBlendAttachment;
	createInfo.renderPass = nullptr; // Always null for now
	return createInfo;
}

vk::GraphicsPipelineCreateInfo VulkanGraphicsPipeline::GetVulkanGraphicsCreateInfo(VulkanGraphicsPipelineStructs& vulkanGraphicsPipelineSpecs)
{
	vk::GraphicsPipelineCreateInfo createInfo{};

	vulkanGraphicsPipelineSpecs.vertexInputInfo.vertexBindingDescriptionCount = vulkanGraphicsPipelineSpecs.vertexInputBindingDescriptions.size();
	vulkanGraphicsPipelineSpecs.vertexInputInfo.pVertexBindingDescriptions = vulkanGraphicsPipelineSpecs.vertexInputBindingDescriptions.data();
	vulkanGraphicsPipelineSpecs.vertexInputInfo.vertexAttributeDescriptionCount = vulkanGraphicsPipelineSpecs.vertexInputAttributeDescriptions.size();
	vulkanGraphicsPipelineSpecs.vertexInputInfo.pVertexAttributeDescriptions = vulkanGraphicsPipelineSpecs.vertexInputAttributeDescriptions.data();

	vulkanGraphicsPipelineSpecs.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(vulkanGraphicsPipelineSpecs.dynamicStates.size());
	vulkanGraphicsPipelineSpecs.dynamicStateInfo.pDynamicStates = vulkanGraphicsPipelineSpecs.dynamicStates.data();

	vulkanGraphicsPipelineSpecs.renderingInfo.colorAttachmentCount = vulkanGraphicsPipelineSpecs.colorAttachmentCount;
	vulkanGraphicsPipelineSpecs.renderingInfo.pColorAttachmentFormats = vulkanGraphicsPipelineSpecs.colorAttachmentFormats.data();
	vulkanGraphicsPipelineSpecs.renderingInfo.depthAttachmentFormat = vulkanGraphicsPipelineSpecs.depthAttachment;

	createInfo.pNext = &vulkanGraphicsPipelineSpecs.renderingInfo;
	createInfo.stageCount = vulkanGraphicsPipelineSpecs.stages.size();
	createInfo.pStages = vulkanGraphicsPipelineSpecs.stages.data();
	createInfo.pVertexInputState = &vulkanGraphicsPipelineSpecs.vertexInputInfo;
	createInfo.pInputAssemblyState = &vulkanGraphicsPipelineSpecs.inputAssemblyState;
	createInfo.pViewportState = &vulkanGraphicsPipelineSpecs.viewportState;
	createInfo.pRasterizationState = &vulkanGraphicsPipelineSpecs.rasterizationState;
	createInfo.pMultisampleState = &vulkanGraphicsPipelineSpecs.multisampleState;

	vulkanGraphicsPipelineSpecs.colorBlendState.attachmentCount = 1;
	vulkanGraphicsPipelineSpecs.colorBlendState.pAttachments = &vulkanGraphicsPipelineSpecs.colorBlend;
	createInfo.pColorBlendState = &vulkanGraphicsPipelineSpecs.colorBlendState;
	createInfo.pDepthStencilState = &vulkanGraphicsPipelineSpecs.depthStencilCreateInfo;
	createInfo.pDynamicState = &vulkanGraphicsPipelineSpecs.dynamicStateInfo;
	createInfo.layout = vulkanGraphicsPipelineSpecs.layout;
	createInfo.renderPass = vulkanGraphicsPipelineSpecs.renderPass;

	return createInfo;
}

vk::VertexInputBindingDescription VulkanGraphicsPipeline::GetBindingDescriptor(const VertexBindingLayout& RHIBindingLayout)
{
	vk::VertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = RHIBindingLayout.binding;
	bindingDescription.stride = RHIBindingLayout.stride;
	bindingDescription.inputRate = TranslateToVulkan(RHIBindingLayout.inputRate);

	return bindingDescription;
}

vk::VertexInputAttributeDescription VulkanGraphicsPipeline::GetAttributeDescriptor(const VertexAttributeLayout& RHiAttributeLayout)
{
	vk::VertexInputAttributeDescription attributeDescription{};
	attributeDescription.binding = RHiAttributeLayout.binding;
	attributeDescription.location = RHiAttributeLayout.location;
	attributeDescription.format = TranslateToVulkan(RHiAttributeLayout.format);
	attributeDescription.offset = RHiAttributeLayout.offset;

	return attributeDescription;
}

END_NAMESPACE_RHI