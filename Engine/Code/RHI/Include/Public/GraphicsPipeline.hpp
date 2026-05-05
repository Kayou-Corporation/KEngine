#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "RHI.hpp"
#include "PipelineCommon.hpp"

BEGIN_NAMESPACE_RHI

class Shader;
class Swapchain;
class PipelineLayout;

struct GraphicsPipelineSpecs
{
	// vk::PipelineRenderingCreateInfo	
	std::vector<Format> colorAttachmentFormats;
	uint32_t colorAttachmentCount;
	Format depthAttachment;

	// CreateInfo
	uint32_t viewportCount = 1;
	uint32_t scissorCount = 1;

	uint32_t lineWidth = 1;
	CullMode cullmode;
	FrontFace frontFace;

	SampleCount SamplesCount;
	bool blendColor; // if true all channel in colorWriteMask

	std::vector<DynamicState> dynamicStates;

	PrimitiveTopology topology;
	std::vector<Core::RefCountPtr<Shader>> shaders{};

	Core::RefCountPtr<PipelineLayout> pipelineLayout;
};

class GraphicsPipeline : public Pipeline
{
public:
};

END_NAMESPACE_RHI