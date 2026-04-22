// unlit.frag.slang

[[vk::binding(1, 1)]]
Sampler2D per_albedoMap;

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
    float4 albedo = per_albedoMap.Sample(input.uv);
    return albedo;
}