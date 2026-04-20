#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

class Window;
class Surface;

struct SwapchainSpecs
{
	Core::RefCountPtr<Surface> surface;

	Extent2D extent;

	uint32_t imageCount;
	PresentMode presentMode;
	
	Format imageFormat;
	
	bool isDepthEnable;
	Format depthImageFormat;
};

class Swapchain : public virtual Core::IResource
{
public:
	virtual ~Swapchain() = default;
};

END_NAMESPACE_RHI