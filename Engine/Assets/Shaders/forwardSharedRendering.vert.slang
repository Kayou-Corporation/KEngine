import "globalLayoutDefinition";

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_Position;

    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldPos : WORLDPOS;
    float3 camPos : CAMPOS;
};

struct Test
{
naea
}
ParameterBlock<Test>

[shader("vertex")]
VSOutput vsMain(VSInput input)
{
    VSOutput o;

    // Transform Vertex position into world pos
    float4 world = mul(globalModels.models[pc.modelID].model, float4(input.position, 1));
    o.worldPos = world.xyz;

    // Transform normal position into world pos
    o.normal = mul((float3x3)globalModels.models[pc.modelID].normalMatrix, input.normal).xyz;

    // Don't transform texCord, keep in UV space
    o.uv = input.uv;

    // Cam pos for lighting purpose in frag (only lit)
    o.camPos = frameData.camera.cameraPos;

    // Out pixel pos in screen
    o.position = mul(frameData.camera.cameraVP, world);

    return o;
}