#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE

class Window;
class Surface;

struct SwapchainSpecs
{
	SurfaceHandle surface;

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

	KENGINE_API virtual uint32_t GetCurrentImageIndex() = 0;
	KENGINE_API virtual uint32_t GetImageCount() = 0;
	KENGINE_API virtual void SwapImages() = 0;

	KENGINE_API Format GetColorImageFormat() const { return m_RHIcolorFormat; }
	KENGINE_API Format GetDepthImageFormat() const { return m_RHIdepthFormat; }

protected:
	Format m_RHIcolorFormat;
	Format m_RHIdepthFormat;
};

END_NAMESPACE_CORE