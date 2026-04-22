// base.vert.slang

import "frame";
import "object";

[[vk::binding(0, 0)]]
ConstantBuffer<CameraUBO> common_camModel;

[[vk::binding(0, 1)]]
ConstantBuffer<ObjectUBO> per_objModel;

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

[shader("vertex")]
VSOutput vsMain(VSInput input)
{
    VSOutput o;

    // Transform Vertex position into world pos
    float4 world = mul(per_objModel.model, float4(input.position, 1));
    o.worldPos = world.xyz;

    // Transform normal position into world pos
    o.normal = mul((float3x3)per_objModel.normalMatrix, input.normal);

    // Don't transform texCord, keep in UV space
    o.uv = input.uv;

    // Cam pos for lighting purpose in frag (only lit)
    o.camPos = common_camModel.camPos;

    // Out pixel pos in screen
    o.position = mul(common_camModel.viewProj, world);

    return o;
}