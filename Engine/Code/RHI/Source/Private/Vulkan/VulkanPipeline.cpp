#include "Private/Vulkan/VulkanPipeline.hpp"

#include "Private/Vulkan/VulkanTranslate.hpp"

BEGIN_NAMESPACE_RHI

VulkanGraphicsPipelineSpecs VulkanPipeline::GetGraphicsCreateInfo(const PipelineSpecs& specs)
{
	VulkanGraphicsPipelineSpecs createInfo{};

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
	if (specs.blendColor)
	{
		colorBlendAttachment.blendEnable = VK_TRUE;
	}
	vk::PipelineColorBlendStateCreateInfo colorBlendingInfo{};
	colorBlendingInfo.attachmentCount = 1;
	colorBlendingInfo.pAttachments = &colorBlendAttachment;

	std::vector<vk::DynamicState> dynamicStates = TranslateToVulkan(specs.dynamicStates);
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates = dynamicStates.data();

	createInfo.colorAttachmentCount = specs.colorAttachmentCount;
	createInfo.colorAttachmentFormats = TranslateToVulkan(specs.colorAttachmentFormats);
	createInfo.depthAttachment = TranslateToVulkan(specs.depthAttachment);
	createInfo.stages = shaderStageInfos;
	createInfo.vertexInputBindingDescriptions = bindingDescriptions;
	createInfo.vertexInputAttributeDescriptions = AttributeDescriptions;
	createInfo.inputAssemblyState = inputAssemblyInfo;
	createInfo.viewportState = viewportStateInfo;
	createInfo.rasterizationState = rasterizationInfo;
	createInfo.multisampleState = multisamplingInfo;
	createInfo.colorBlendState = colorBlendingInfo;
	createInfo.dynamicState = dynamicStateInfo;
	createInfo.layout = m_layout;
	createInfo.renderPass = nullptr; // Always null for now


	return createInfo;
}

vk::GraphicsPipelineCreateInfo VulkanPipeline::GetVulkanGraphicsCreateInfo(const VulkanGraphicsPipelineSpecs& vulkanGraphicsPipelineSpecs)
{
	vk::GraphicsPipelineCreateInfo createInfo{};

	vk::PipelineRenderingCreateInfo renderingInfo{};
	renderingInfo.colorAttachmentCount = vulkanGraphicsPipelineSpecs.colorAttachmentCount;
	renderingInfo.pColorAttachmentFormats = vulkanGraphicsPipelineSpecs.colorAttachmentFormats.data();
	renderingInfo.depthAttachmentFormat = vulkanGraphicsPipelineSpecs.depthAttachment;

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.vertexBindingDescriptionCount = vulkanGraphicsPipelineSpecs.vertexInputBindingDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = vulkanGraphicsPipelineSpecs.vertexInputBindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = vulkanGraphicsPipelineSpecs.vertexInputAttributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = vulkanGraphicsPipelineSpecs.vertexInputAttributeDescriptions.data();

	createInfo.pNext = &renderingInfo;
	createInfo.stageCount = vulkanGraphicsPipelineSpecs.stages.size();
	createInfo.pStages = vulkanGraphicsPipelineSpecs.stages.data();
	createInfo.pVertexInputState = &vertexInputInfo;
	createInfo.pInputAssemblyState = &vulkanGraphicsPipelineSpecs.inputAssemblyState;
	createInfo.pViewportState = &vulkanGraphicsPipelineSpecs.viewportState;
	createInfo.pRasterizationState = &vulkanGraphicsPipelineSpecs.rasterizationState;
	createInfo.pMultisampleState = &vulkanGraphicsPipelineSpecs.multisampleState;
	createInfo.pColorBlendState = &vulkanGraphicsPipelineSpecs.colorBlendState;
	createInfo.pDynamicState = &vulkanGraphicsPipelineSpecs.dynamicState;
	createInfo.layout = vulkanGraphicsPipelineSpecs.layout;
	createInfo.renderPass = vulkanGraphicsPipelineSpecs.renderPass;

	return createInfo;
}

vk::ComputePipelineCreateInfo VulkanPipeline::GetComputeCreateInfo(const PipelineSpecs& specs)
{
	// Not Implemented yet
	(void)specs;
	vk::ComputePipelineCreateInfo createInfo{};

	return createInfo;
}

vk::PipelineLayoutCreateInfo VulkanPipeline::GetPipelineLayoutCreateInfo()
{
	vk::PipelineLayoutCreateInfo createInfo{};

	if (!m_descriptors.empty())
	{
		createInfo.setSetLayoutCount(static_cast<uint32_t>(m_descriptors.size()));
		createInfo.setPSetLayouts(m_descriptors.data());
	}
	else
	{
		createInfo.setSetLayoutCount(0);
		createInfo.setPSetLayouts(nullptr);
	}

	return createInfo;
}

vk::VertexInputBindingDescription VulkanPipeline::GetBindingDescriptor(const VertexBindingLayout& RHIBindingLayout)
{
	vk::VertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = RHIBindingLayout.binding;
	bindingDescription.stride = RHIBindingLayout.stride;
	bindingDescription.inputRate = TranslateToVulkan(RHIBindingLayout.inputRate);

	return bindingDescription;
}

vk::VertexInputAttributeDescription VulkanPipeline::GetAttributeDescriptor(const VertexAttributeLayout& RHiAttributeLayout)
{
	vk::VertexInputAttributeDescription attributeDescription{};
	attributeDescription.binding = RHiAttributeLayout.binding;
	attributeDescription.location = RHiAttributeLayout.location;
	attributeDescription.format = TranslateToVulkan(RHiAttributeLayout.format);
	attributeDescription.offset = RHiAttributeLayout.offset;

	return attributeDescription;
}

std::vector<VulkanDescriptorSetLayoutSpecs> VulkanPipeline::GetDescriptorSetLayoutCreateInfo(std::vector<Descriptor>& RHIDescriptors)
{
	std::vector<VulkanDescriptorSetLayoutSpecs> layoutsSpecs{};
	layoutsSpecs.reserve(RHIDescriptors.size());
	for (const Descriptor& RHIDescriptor : RHIDescriptors)
	{
		VulkanDescriptorSetLayoutSpecs layoutSpec{};
		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		for (const Binding& RHIBinding : RHIDescriptor.bindings)
		{
			vk::DescriptorSetLayoutBinding binding{};
			binding.binding = RHIBinding.index;
			binding.descriptorType = TranslateToVulkan(RHIBinding.type);
			binding.descriptorCount = RHIBinding.count;
			binding.stageFlags = TranslateToVulkan(RHIBinding.stage);

			layoutSpec.bindings.push_back(binding);
		}

		layoutsSpecs.push_back(layoutSpec);
	}

	return layoutsSpecs;
}

std::vector<vk::DescriptorSetLayoutCreateInfo> VulkanPipeline::GetVulkanDescriptorSetLayoutCreateInfo(std::vector<VulkanDescriptorSetLayoutSpecs>& RHIVulkanDescriptorSetLayoutSpecs)
{
	std::vector<vk::DescriptorSetLayoutCreateInfo> layoutsInfo{};

	for (const auto& layoutSpec : RHIVulkanDescriptorSetLayoutSpecs)
	{
		vk::DescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.bindingCount = layoutSpec.bindings.size();
		layoutInfo.pBindings = layoutSpec.bindings.data();

		layoutsInfo.push_back(layoutInfo);
	}

	return layoutsInfo;
}

END_NAMESPACE_RHI