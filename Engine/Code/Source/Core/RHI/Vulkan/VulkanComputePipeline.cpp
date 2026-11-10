#include "RHI/Vulkan/VulkanComputePipeline.hpp"
#include "RHI/Vulkan/VulkanPipelineCommon.hpp"

#include "RHI/Vulkan/VulkanTranslate.hpp"

BEGIN_NAMESPACE_CORE

vk::ComputePipelineCreateInfo VulkanComputePipeline::GetVulkanComputeCreateInfo(const ComputePipelineSpecs& specs) const
{
    const Core::KSharedPtr<VulkanShader> RHIVulkanShader = specs.shader.CastAs<VulkanShader>();
    const ShaderStage stage = specs.shader->GetShaderStage();

    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo{};
    shaderStageCreateInfo.stage = TranslateToVulkan(stage);
    shaderStageCreateInfo.module = RHIVulkanShader->GetModule();
    shaderStageCreateInfo.pName = "main";

    vk::ComputePipelineCreateInfo createInfo;
    createInfo.layout = m_layout.CastAs<VulkanPipelineLayout>()->GetHandle();
    createInfo.stage = shaderStageCreateInfo;

    return createInfo;
}

END_NAMESPACE_CORE
