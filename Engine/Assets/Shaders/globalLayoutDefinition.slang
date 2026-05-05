// globalLayoutDefinition.slang

// Set O --- FrameData ---
struct Camera
{
    float4x4 CameraVP;
    float3 CameraPos;
};

struct Light
{
    float3 pos;
    float3 dir;
    float intensity;
    int type;
};

struct FrameData
{
    ConstantBuffer<Camera> camera; // UniformBuffer
    StructuredBuffer<Light> lights; // SSBO (StorageBuffer)
};
ParameterBlock<FrameData> frameData;

// Set 1 --- DrawData ---
struct DrawData
{
    ConstantBuffer<float4x4> model;        // UniformBuffer
    ConstantBuffer<float4x4> normalMatrix; // UniformBuffer
    Sampler2D texture2D;
};
ParameterBlock<DrawData> drawData;