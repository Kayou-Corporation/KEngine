#pragma once

#include "RHI/API/RHI.hpp"
#include "RHI/API/PipelineCommon.hpp"

BEGIN_NAMESPACE_CORE

struct GraphicsPipelineColorBlendAttachment
{
	bool blendColor; // if true all channel in colorWriteMask
	BlendFactor srcColorBlendFactor = BlendFactor::Zero;
	BlendFactor dstColorBlendFactor = BlendFactor::Zero;
	BlendOperation colorBlendOperation = BlendOperation::Add;
	BlendFactor srcAlphaBlendFactor = BlendFactor::Zero;
	BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
	BlendOperation alphaBlendOperation = BlendOperation::Add;
	ColorComponentFlags colorWriteMask;
};

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
	std::vector<GraphicsPipelineColorBlendAttachment> colorBlendAttachements;

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