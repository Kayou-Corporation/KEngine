#pragma once

#include "Public/GraphicsPipeline.hpp"

#include "VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanGraphicsPipeline : public GraphicsPipeline
{
public:
	VulkanGraphicsPipeline() = default;
	virtual ~VulkanGraphicsPipeline() override = default;

public:
	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> GetCreateInfo(const PipelineSpecs& specs);

	vk::Pipeline GetHandle() const { return m_handle; }

	void SetHandle(vk::Pipeline handle) { m_handle = handle; }

private:
	vk::Pipeline m_handle;
};

END_NAMESPACE_RHI