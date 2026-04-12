// unlit.frag.slang

import "frame";
import "object";

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
    float4 albedo = object.material.albedoMap.Sample(input.uv);
    return albedo;
}