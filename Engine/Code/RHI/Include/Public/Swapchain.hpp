#pragma once

#include "Core/Utils/Export.hpp"
#include "Core/Utils/Memory.hpp"

#include "Core/RHI/Public/RHI.hpp"

BEGIN_NAMESPACE_CORE

class Window;
class Surface;

struct SwapchainSpecs
{
	RefCountPtr<Surface> surface;

	Extent2D extent;

	uint32_t imageCount;
	PresentMode presentMode;
	
	Format imageFormat;
	
	bool isDepthEnable;
	Format depthImageFormat;
};

class Swapchain : public virtual IResource
{
public:
	virtual ~Swapchain() = default;
};

END_NAMESPACE_CORE