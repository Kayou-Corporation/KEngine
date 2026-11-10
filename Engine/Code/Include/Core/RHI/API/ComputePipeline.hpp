#pragma once

#include "RHI/API/RHI.hpp"
#include "RHI/API/PipelineCommon.hpp"

BEGIN_NAMESPACE_CORE

struct ComputePipelineSpecs
{
    ShaderHandle shader;
    PipelineLayoutHandle pipelineLayout;
};

class ComputePipeline : public Pipeline
{
public:
};

END_NAMESPACE_CORE