// globalLayout.slang

struct CameraUBO
{
    float4x4 CameraVP;
    float3 CameraPos;
};

struct Light
{
    float3 pos;
    float intensity;
}

// Set O -- FrameData ---
[[vk::binding(0, 0)]] ConstantBuffer<CameraUBO> cameraUbo;
[[vk::binding(1, 0)]] StructuredBuffer<Light> lightSSB0;

// Set 1 -- DrawData
[[vk::binding(0, 1)]] ConstantBuffer<float4x4> objectModel;
[[vk::binding(1, 1)]] int ObjectID;
[[vk::binding(2, 1)]] Sampler2D Texture;

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