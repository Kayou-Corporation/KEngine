// globalLayout.slang

// The only purpose of this shader is to be compiled to reflect the global layout, not to be used for rendering

import "globalLayoutDefinition";

struct VSInput
{
};

struct VSOutput
{
    float4 position : SV_Position;
};

[shader("vertex")]
VSOutput vsMain(VSInput input)
{
    VSOutput o;
    o.position = float4(0.f, 0.f, 0.f, 1.f);
    return o;
}