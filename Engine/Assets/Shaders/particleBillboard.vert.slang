// particleBillboart.vert.slang

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

static const float2 kQuadOffsets[6] =
{
    float2(-0.5, -0.5),
    float2( 0.5, -0.5),
    float2( 0.5,  0.5),

    float2(-0.5, -0.5),
    float2( 0.5,  0.5),
    float2(-0.5,  0.5)
};

[shader("vertex")]
VSOutput vsMain(uint vertexID : SV_VulkanVertexID, uint instanceID : SV_VulkanInstanceID)
{
    VSOutput output;

    float2 offset = kQuadOffsets[vertexID];
    output.uv = offset + 0.5;

    float3 centerWorldPos = particleData.positions[instanceID].xyz;
    float4 clipPos = mul(frameData.camera.cameraVP, float4(centerWorldPos, 1.0));

    float particleSize = 4.0;
    float aspectRatio = 16.0 / 9.0; // maybe with push constant or idk

    clipPos.xy += offset * particleSize * float2(1.0 / aspectRatio, 1.0);

    output.position = clipPos;
    return output;
}