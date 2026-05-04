// globalLayout.slang

struct CameraUBO
{
    float4x4 CameraVP;
    float3 CameraPos;
};

struct FrameDataSet
{
    CameraUBO CameraUBO;
    float4x4 ObjectModel;
    int ObjectID;
}

ParameterBlock<FrameDataSet> frameDataSet;

struct VSInput
{
};

struct VSOutput
{
};

[shader("vertex")]
VSOutput vsMain(VSInput input)
{

    
    VSOutput o;
    return o;
}