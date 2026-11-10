#pragma once

#include "RHI/API/ComputePipeline.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"
#include "RHI/Vulkan/VulkanShader.hpp"

BEGIN_NAMESPACE_CORE

class VulkanComputePipeline : public ComputePipeline
{
public:
    VulkanComputePipeline() = default;
    virtual ~VulkanComputePipeline() override = default;

public:
    //VulkanGraphicsPipelineStructs GetGraphicsCreateInfo(const GraphicsPipelineSpecs& specs);
    //vk::GraphicsPipelineCreateInfo GetVulkanGraphicsCreateInfo(VulkanGraphicsPipelineStructs& vulkanGraphicsPipelineSpecs);

    vk::VertexInputBindingDescription GetBindingDescriptor(const VertexBindingLayout& RHIBindingLayout);
    vk::VertexInputAttributeDescription GetAttributeDescriptor(const VertexAttributeLayout& RHiAttributeLayout);

    vk::Pipeline GetHandle() const { return m_handle; }

    void SetHandle(vk::Pipeline handle) { m_handle = handle; }

private:
    vk::Pipeline m_handle;
};

END_NAMESPACE_CORE