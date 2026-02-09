#pragma once

#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#include "Core/RHI/Public/RHI.hpp"

class Swapchain;
struct SwapchainSpecs;

struct DeviceSpecs
{
	GpuType gpuType;
	std::vector<Extensions> extensions;
	std::vector<Features> features;

	std::vector<Queue> queue;
};

class Device : public virtual IResource
{
public:
	virtual ~Device() = default;

	virtual void Create(const DeviceSpecs specs) = 0;
	virtual void Destroy() = 0;

	//virtual RefCountPtr<Swapchain>  CreateSwapchain(const SwapchainSpecs& specs) = 0
	//virtual void DestroySwapchain(RefCountPtr<Swapchain> swapchain) = 0
};