#pragma once

#include <string>
#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#include "Core/RHI/Public/RHI.hpp"

// Forward declaration
class Window;
class Surface;


struct InstanceSpecs
{
	RefCountPtr<Window> window; // To get window specific extensions required.

	Version appVersion;
	Version engineVersion;

	std::vector<Extensions> extensions;   // Extensions to check their availability
	std::vector<DebugLayers> debugLayers; // Debug layers to use.
};

class Instance : public IResource
{
public:
	virtual ~Instance() = default;

	KENGINE_API virtual void Create(const InstanceSpecs& specs) = 0;
	KENGINE_API virtual void Destroy() = 0;

	//KENGINE_API virtual RefCountPtr<Surface> CreateSurface(RefCountPtr<Window> window) = 0;
	//KENGINE_API virtual void DestroySurface(RefCountPtr<Surface>) = 0;

protected:
	Version m_appVersion;
	Version m_engineVersion;
};