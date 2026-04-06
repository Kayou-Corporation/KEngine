// object.slang

import "frame";

struct ObjectParams
{
    float4x4 model;
    float4x4 normalMat;
};

struct MaterialParams
{
    Sampler2D albedoMap;
};

ParameterBlock<FrameParams> frame;
ParameterBlock<ObjectParams> object;
ParameterBlock<MaterialParams> material;