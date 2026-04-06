#pragma once

#include <vector>

#include "Utils/Memory.hpp"
#include "Utils/Export.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

class Surface;

class Swapchain;
struct SwapchainSpecs;

class Buffer;
struct BufferSpecs;

class Image;
struct ImageSpecs;
struct SwapchainImageSpecs;

class CommandList;

struct DeviceSpecs
{
	GpuType gpuType;
	std::vector<Features> features; // Not implemented 

	std::vector<Extensions> extensions;
	std::vector<QueueType> queues;
	bool searchPresentQueue;
	Core::RefCountPtr<Surface> surface;
};

class Device : public virtual Core::IResource
{
public:
	virtual ~Device() = default;

	KENGINE_API virtual Core::RefCountPtr<CommandList> GetCommandList(QueueType type) = 0;
	KENGINE_API virtual void SubmitCommandList(Core::RefCountPtr<CommandList> commandList) = 0;
	KENGINE_API virtual void WaitIdle() = 0;
	KENGINE_API virtual void QueueWaitIdle(QueueType type) = 0;
	KENGINE_API virtual void RunGarbageCollector() = 0;

	// -------- Create -------- // 
	
	// ----- Swapchain ------- // 
	KENGINE_API virtual Core::RefCountPtr<Swapchain> CreateSwapchain(const SwapchainSpecs& specs) = 0;
	KENGINE_API virtual void DestroySwapchain(Core::RefCountPtr<Swapchain> swapchain) = 0;

	// ----- Buffer ------- // 
	KENGINE_API virtual Core::RefCountPtr<Buffer> CreateBuffer(const BufferSpecs& specs) = 0;
	KENGINE_API virtual void DestroyBuffer(Core::RefCountPtr<Buffer> buffer) = 0;

	// ----- Image ------- // 
	KENGINE_API virtual Core::RefCountPtr<Image> CreateImage(const ImageSpecs& specs) = 0;
	KENGINE_API virtual void DestroyImage(Core::RefCountPtr<Image> image) = 0;
	// ------ Only for color image -------- // 
	KENGINE_API virtual std::vector<Core::RefCountPtr<Image>> CreatePresentationImages(Core::RefCountPtr<Swapchain> swapchain) = 0;
	KENGINE_API virtual void DestroyPresentationImages(std::vector<Core::RefCountPtr<Image>> presentationImages) = 0;
	// use classic destroy image
	KENGINE_API virtual Core::RefCountPtr<Image> CreateImagesWithSwapchain(const SwapchainImageSpecs& specs, Core::RefCountPtr<Swapchain> swapchain) = 0;

};

END_NAMESPACE_RHI