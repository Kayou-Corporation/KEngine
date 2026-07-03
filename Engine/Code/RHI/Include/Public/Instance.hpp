#pragma once

#include <string>
#include <vector>

#include "Window/Window.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct InstanceSpecs
{
	Core::RefCountPtr<Core::Window> window; // To get window specific extensions required.

	Version appVersion;
	Version engineVersion;

	std::vector<DebugLayers> debugLayers; // Debug layers to use.
};

class Instance : virtual public Core::IResource
{
public:
	virtual ~Instance() = default;

	KENGINE_API virtual void Create(const InstanceSpecs& specs) = 0;
	KENGINE_API virtual void Destroy() = 0;

	KENGINE_API virtual Core::RefCountPtr<Surface> CreateSurface(const SurfaceSpecs& specs) = 0;
	KENGINE_API virtual void DestroySurface(Core::RefCountPtr<Surface> surface) = 0;

	KENGINE_API virtual DeviceHandle CreateDevice(const DeviceSpecs& specs) = 0;
	KENGINE_API virtual void DestroyDevice(DeviceHandle device) = 0;

protected:
	Version m_appVersion;
	Version m_engineVersion;
};

class RendererInterface
{
public:
	static InstanceHandle InitRenderer(Core::RendererAPI api);
};

END_NAMESPACE_RHI