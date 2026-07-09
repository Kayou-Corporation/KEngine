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

	KAPI virtual uint32_t GetCurrentImageIndex() = 0;
	KAPI virtual uint32_t GetImageCount() = 0;
	KAPI virtual void SwapImages() = 0;

	KAPI Format GetColorImageFormat() const { return m_RHIcolorFormat; }
	KAPI Format GetDepthImageFormat() const { return m_RHIdepthFormat; }

protected:
	Format m_RHIcolorFormat;
	Format m_RHIdepthFormat;
};

END_NAMESPACE_CORE