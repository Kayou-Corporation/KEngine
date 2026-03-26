#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct ImageSpecs
{
	Format format;
	Layout finalLayout;

	ImageType type;
	std::vector<ImageUsage> usages;

	Extent3D extent;

	uint32_t layersCount = 1;
	uint32_t channelsCount = 4;

	uint32_t mipLevels = 1;
	SampleCount sampleCount = SampleCount::Count1;
};

class Image : virtual public Core::IResource
{
public:
	virtual ~Image() = default;


};

END_NAMESPACE_RHI