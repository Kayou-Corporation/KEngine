#include "Private/Vulkan/VulkanGraphicsPipeline.hpp"

#include "Private/Vulkan/VulkanTranslate.hpp"
#include "Private/Vulkan/VulkanShader.hpp"

BEGIN_NAMESPACE_RHI

vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> VulkanGraphicsPipeline::GetCreateInfo(const PipelineSpecs& specs)
{
	std::size_t stageCount = specs.stageCount;

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos{};
	shaderStageInfos.reserve(stageCount);

	for (const auto& shader : specs.shaders)
	{
		Core::RefCountPtr<VulkanShader> vulkanShader = shader.CastAs<VulkanShader>();
		ShaderStage stage = shader->GetShaderStage();
		const char* entryName = ShaderStageToEntry(stage);

		vk::PipelineShaderStageCreateInfo shaderStageCreateInfo{};
		shaderStageCreateInfo.stage = TranslateToVulkan(stage);
		shaderStageCreateInfo.module = vulkanShader->GetModule();
		shaderStageCreateInfo.pName = entryName;

		shaderStageInfos.push_back(shaderStageCreateInfo);
	}

	return vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo>();
}

END_NAMESPACE_RHI