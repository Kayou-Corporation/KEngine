#pragma once

#include "RHI/API/RHI.hpp"
#include "RHI/API/PipelineCommon.hpp"

BEGIN_NAMESPACE_CORE

struct GraphicsPipelineSpecs
{
	// vk::PipelineRenderingCreateInfo	
	std::vector<Format> colorAttachmentFormats;
	uint32_t colorAttachmentCount;
	Format depthAttachment;

	// CreateInfo
	uint32_t viewportCount = 1;
	uint32_t scissorCount = 1;

	float lineWidth = 1.f;
	CullMode cullmode;
	FrontFace frontFace;

	SampleCount SamplesCount;
	bool blendColor; // if true all channel in colorWriteMask

	std::vector<DynamicState> dynamicStates;

	PrimitiveTopology topology;

	bool depthTest;
	bool depthWrite;
	CompareOp depthCompare;

	std::vector<ShaderHandle> shaders{};

	PipelineLayoutHandle pipelineLayout;
};

class GraphicsPipeline : public Pipeline
{
public:
};

END_NAMESPACE_CORE