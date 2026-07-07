#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct SamplerSpecs 
{
    Filter magFilter = Filter::Linear;
    Filter minFilter = Filter::Linear;
    SamplerMipmapMode mipmapMode = SamplerMipmapMode::Linear;

    SamplerAddressMode addressU = SamplerAddressMode::Repeat;
    SamplerAddressMode addressV = SamplerAddressMode::Repeat;
    SamplerAddressMode addressW = SamplerAddressMode::Repeat;

    float mipLodBias = 0.0f;
    bool anisotropyEnable = false;
    float maxAnisotropy = 1.0f;

    bool compareEnable = false;
    CompareOp compareOp = CompareOp::Always;

    float minLod = 0.0f;
    float maxLod = 0.0f;
    BorderColor borderColor = BorderColor::IntOpaqueBlack;
    bool unnormalizedCoordinates = false;
};

class Sampler : public virtual Core::IResource
{
public:
	virtual ~Sampler() = default;


};

END_NAMESPACE_RHI