#pragma once

#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#include "Core/RHI/Public/RHI.hpp"

BEGIN_NAMESPACE_CORE

class Surface;

class Swapchain;
struct SwapchainSpecs;

class Buffer;
struct BufferSpecs;

struct DeviceSpecs
{
	GpuType gpuType;
	std::vector<Features> features; // Not implemented 

	std::vector<Extensions> extensions;
	std::vector<QueueType> queues;
	bool searchPresentQueue;
	RefCountPtr<Surface> surface;
};

class Device : public virtual IResource
{
public:
	virtual ~Device() = default;

	KENGINE_API virtual void WaitIdle() = 0;
	KENGINE_API virtual void QueueWaitIdle(QueueType type) = 0;


	KENGINE_API virtual RefCountPtr<Swapchain> CreateSwapchain(const SwapchainSpecs& specs) = 0;
	KENGINE_API virtual void DestroySwapchain(RefCountPtr<Swapchain> buffer) = 0;

	KENGINE_API virtual RefCountPtr<Buffer> CreateBuffer(const BufferSpecs& specs) = 0;
	KENGINE_API virtual void DestroyBuffer(RefCountPtr<Buffer> buffer) = 0;
};

END_NAMESPACE_CORE