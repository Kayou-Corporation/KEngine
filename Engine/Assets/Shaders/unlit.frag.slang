// unlit.frag.slang

import "globalLayoutDefinition";

struct FSInput
{
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldPos : WORLDPOS;
    float3 camPos : CAMPOS;
};

[shader("fragment")]
float4 fsMain(FSInput input) : SV_Target
{
    float4 albedo = drawData.texture2D.Sample(drawData.sampler, input.uv);
    //float4 albedo = float4(input.normal, 1.f);
    return albedo;
}