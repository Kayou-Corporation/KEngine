// globalLayoutDefinition.slang

// Set O --- FrameData ---
struct Camera
{
    float4x4 cameraVP;
    float3 cameraPos;
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

struct Object
{
    float4x4 model;
    float4x4 normalMatrix;
}
// Set 1 --- DrawData ---
struct DrawData
{
    ConstantBuffer<Object> object;
    //ConstantBuffer<float4x4> model;        // UniformBuffer
    //ConstantBuffer<float4x4> normalMatrix; // UniformBuffer
    Texture2D texture2D;
    SamplerState sampler;
};
ParameterBlock<DrawData> drawData;

//struct PushConstants
//{
//     int textureID;
//     int modelID;
//}
//
//[push_constant]
//ConstantBuffer<PushConstants> pushConstants;