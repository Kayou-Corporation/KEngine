// frame.slang

struct FrameParams
{
    float4x4 viewProj;
    float3 camPos;
};

ParameterBlock<FrameParams> frame;