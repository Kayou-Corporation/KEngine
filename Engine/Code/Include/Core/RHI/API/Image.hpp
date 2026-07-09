#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE

struct ImageSpecs
{
	Format format;
	Layout targetLayout;

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

	ImageType type;
	ImageViewType viewType;
	ImageViewAspect viewAspect;
	std::vector<ImageUsage> usages;
};

class Image : virtual public Core::IResource
{
public:
	virtual ~Image() = default;

	KAPI Extent3D GetExtent() { return m_rhiExtent; }
	KAPI Format GetFormat() { return m_rhiFormat; }
	KAPI Layout GetCurrentLayout() { return m_rhiCurrentLayout; }
	KAPI Layout GetTargetLayout() { return m_rhiTargetLayout; }

protected:
	Extent3D m_rhiExtent;
	Format m_rhiFormat;
	Layout m_rhiTargetLayout;

	Layout m_rhiCurrentLayout = Layout::Undefined;
};

END_NAMESPACE_CORE