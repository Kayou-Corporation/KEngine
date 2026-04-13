#pragma once

#include "Public/Pipeline.hpp"

#include "VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanPipeline : public Pipeline
{
public:
	VulkanPipeline() = default;
	virtual ~VulkanPipeline() override = default;

public:
	vk::GraphicsPipelineCreateInfo GetGraphicsCreateInfo(const PipelineSpecs& specs);
	vk::ComputePipelineCreateInfo GetComputeCreateInfo(const PipelineSpecs& specs);

	vk::Pipeline GetHandle() const { return m_handle; }

	void SetHandle(vk::Pipeline handle) { m_handle = handle; }

private:
	vk::Pipeline m_handle;
};

END_NAMESPACE_RHI