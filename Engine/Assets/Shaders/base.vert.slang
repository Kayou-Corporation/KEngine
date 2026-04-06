// base.vert.slang

import "object";

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

    float4 world = mul(object.model, float4(input.position, 1));

    o.worldPos = world.xyz;

    o.normal = mul((float3x3)object.normalMat, input.normal);

    o.uv = input.uv;

    o.camPos = frame.camPos;

    o.position = mul(frame.viewProj, world);

    return o;
}