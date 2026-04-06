// object.slang

import "frame";

struct ObjectParams
{
    float4x4 model;
    float4x4 normalMat;
};

struct MaterialParams
{
    Texture2D albedoMap;
};

ParameterBlock<ObjectParams> object;
ParameterBlock<MaterialParams> material;