// globalLayout.slang

// The only purpose of this shader is to be compiled to reflect the global layout, not to be used for rendering

import "globalLayoutDefinition";

struct VSInput
{
};

struct VSOutput
{
};

[shader("vertex")]
VSOutput vsMain(VSInput input)
{
    VSOutput o;

    return o;
}