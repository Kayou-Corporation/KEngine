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
    vk::ComputePipelineCreateInfo GetVulkanComputeCreateInfo(const ComputePipelineSpecs& specs) const;

    vk::Pipeline GetHandle() const { return m_handle; }

    void SetHandle(const vk::Pipeline handle) { m_handle = handle; }

private:
    vk::Pipeline m_handle;
};

END_NAMESPACE_CORE
