#pragma once

#include <string>
#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#include "Core/RHI/Public/RHI.hpp"

BEGIN_NAMESPACE_CORE

// Forward declaration
class Window;

class Surface;
struct SurfaceSpecs;

class Device;
struct DeviceSpecs;

struct InstanceSpecs
{
	RefCountPtr<Window> window; // To get window specific extensions required.

	Version appVersion;
	Version engineVersion;

	std::vector<DebugLayers> debugLayers; // Debug layers to use.
};

class Instance : virtual public IResource
{
public:
	virtual ~Instance() = default;

	KENGINE_API virtual void Create(const InstanceSpecs& specs) = 0;
	KENGINE_API virtual void Destroy() = 0;

	KENGINE_API virtual RefCountPtr<Surface> CreateSurface(const SurfaceSpecs& specs) = 0;
	KENGINE_API virtual void DestroySurface(RefCountPtr<Surface> surface) = 0;

	KENGINE_API virtual RefCountPtr<Device> CreateDevice(const DeviceSpecs& specs) = 0;
	KENGINE_API virtual void DestroyDevice(RefCountPtr<Device> device) = 0;

protected:
	Version m_appVersion;
	Version m_engineVersion;
};

class RendererInterface
{
public:
	static RefCountPtr<Instance> InitRenderer(RendererAPI api);
};

END_NAMESPACE_CORE