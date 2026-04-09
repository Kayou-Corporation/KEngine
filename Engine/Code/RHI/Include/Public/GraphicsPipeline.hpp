#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "RHI.hpp"

BEGIN_NAMESPACE_RHI

class Shader;
class Swapchain;

struct PipelineSpecs
{
	Core::RefCountPtr<Swapchain> swapchain{};
	std::size_t stageCount = 0;
	std::vector<Core::RefCountPtr<Shader>> shaders{};

};

class GraphicsPipeline : virtual public Core::IResource
{
public:
	virtual ~GraphicsPipeline() = default;

};

END_NAMESPACE_RHI