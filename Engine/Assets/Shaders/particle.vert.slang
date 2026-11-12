// particle.vert.slang

import "globalLayoutDefinition";

struct ParticleInput
{
    StructuredBuffer<float4> positions;
};
ParameterBlock<ParticleInput> particleInput;

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

float4x4 CreateModelMatrix(float4 position)
{
    return float4x4(
        1.0,    0.0,    0.0,    position.x,
        0.0,    1.0,    0.0,    position.y,
        0.0,    0.0,    1.0,    position.z,
        0.0,    0.0,    0.0,    1.0
    );
}

[shader("vertex")]
VSOutput vsMain(VSInput input, uint instanceId : SV_InstanceID)
{
    VSOutput o;

    float4x4 model = CreateModelMatrix(particleInput.positions[instanceId]);

    float4 world = mul(model, float4(input.position, 1));
    o.worldPos = world.xyz;

    o.uv = input.uv;

    o.normal = normalize(mul((float3x3)model, input.normal));

    o.camPos = frameData.camera.cameraPos;

    o.position = mul(frameData.camera.cameraVP, world);

    return o;
}