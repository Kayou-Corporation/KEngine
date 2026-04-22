#include "Private/Vulkan/VulkanPipeline.hpp"

#include "Private/Vulkan/VulkanTranslate.hpp"
#include "Private/Vulkan/VulkanShader.hpp"

BEGIN_NAMESPACE_RHI

vk::GraphicsPipelineCreateInfo VulkanPipeline::GetGraphicsCreateInfo(const PipelineSpecs& specs)
{
	vk::GraphicsPipelineCreateInfo createInfo{};

	std::vector<vk::Format> colorAttachmentFormats = TranslateToVulkan(specs.colorAttachmentFormats);
	vk::PipelineRenderingCreateInfo renderingInfo{};
	renderingInfo.colorAttachmentCount = specs.colorAttachmentCount;
	renderingInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
	renderingInfo.depthAttachmentFormat = TranslateToVulkan(specs.depthAttachment);

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos{};
	shaderStageInfos.reserve(specs.shaders.size());
	for (const auto& RHIShader : specs.shaders)
	{
		Core::RefCountPtr<VulkanShader> RHIVulkanShader = RHIShader.CastAs<VulkanShader>();
		ShaderStage stage = RHIShader->GetShaderStage();
		const char* entryName = ShaderStageToEntry(stage);

		vk::PipelineShaderStageCreateInfo shaderStageCreateInfo{};
		shaderStageCreateInfo.stage = TranslateToVulkan(stage);
		shaderStageCreateInfo.module = RHIVulkanShader->GetModule();
		shaderStageCreateInfo.pName = entryName;

		shaderStageInfos.push_back(shaderStageCreateInfo);
	}

	// TODO : Probably should deduce with shaders, for now empty
	[[maybe_unused]] vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

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

	[[maybe_unused]] vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};

	createInfo.pNext = &renderingInfo;
	createInfo.stageCount = shaderStageInfos.size();
	createInfo.pStages = shaderStageInfos.data();
	//createInfo.pVertexInputState = &vertexInputInfo
	createInfo.pInputAssemblyState = &inputAssemblyInfo;
	createInfo.pViewportState = &viewportStateInfo;
	createInfo.pRasterizationState = &rasterizationInfo;
	createInfo.pMultisampleState = &multisamplingInfo;
	createInfo.pColorBlendState = &colorBlendingInfo;
	createInfo.pDynamicState = &dynamicStateInfo;
	//createInfo.layout = pipelineLayout;
	createInfo.renderPass = nullptr; // Always null for now


	return createInfo;
}

vk::ComputePipelineCreateInfo VulkanPipeline::GetComputeCreateInfo(const PipelineSpecs& specs)
{
	// Not Implemented yet
	(void)specs;
	vk::ComputePipelineCreateInfo createInfo{};

	return createInfo;
}

END_NAMESPACE_RHI