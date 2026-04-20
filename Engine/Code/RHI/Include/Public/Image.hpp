#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct ImageSpecs
{
	ImageSource source;

	Format format;
	Layout targetLayout;
	Layout finalLayout;

	ImageType type;
	ImageViewType viewType;
	ImageViewAspect viewAspect;
	std::vector<ImageUsage> usages;

	Extent3D extent;

	uint32_t layersCount = 1;

	uint32_t mipLevels = 1;
	SampleCount sampleCount = SampleCount::Count1;
};

enum class SwapchainImageType
{
	Color,
	Depth
};

// Fill in device with informations read from swapchain mostly internal to image ?
struct SwapchainImageSpecs
{
	// To see if you will use the swapchain color or depth format
	SwapchainImageType imageType;

	Layout targetLayout;
	Layout finalLayout;

	ImageType type;
	ImageViewType viewType;
	ImageViewAspect viewAspect;
	std::vector<ImageUsage> usages;
};

class Image : virtual public Core::IResource
{
public:
	virtual ~Image() = default;

	KENGINE_API ImageSource GetSource() { return m_source; }

protected:
	ImageSource m_source;
};

END_NAMESPACE_RHI