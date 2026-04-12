// object.slang

struct ObjectParams
{
    float4x4 model;
    float4x4 normalMat;
};

struct MaterialParams
{
    Sampler2D albedoMap;
};

struct FullObjectParams
{
    ObjectParams object;
    MaterialParams material;
};

ParameterBlock<FullObjectParams> object;
//ParameterBlock<ObjectParams> object;
//ParameterBlock<MaterialParams> material;