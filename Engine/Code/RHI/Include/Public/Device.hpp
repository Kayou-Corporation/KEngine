#pragma once

#include <vector>

#include "Utils/Memory.hpp"
#include "Utils/Export.hpp"

#include "Public/RHI.hpp"
#include "Public/Shader.hpp"

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

class Semaphore;
struct SemaphoreSpecs;
class Fence;
struct SubmitInfo;

class Pipeline;
struct PipelineSpecs;

struct DeviceSpecs
{
	GpuType gpuType;
	std::vector<Features> features; // Not implemented 

	std::vector<Extensions> extensions;
	std::vector<QueueType> queues;
	bool searchPresentQueue;
	Core::RefCountPtr<Surface> surface;
};

struct PresentInfo
{
	std::vector<Core::RefCountPtr<Semaphore>> waitSemaphores;
	std::vector<uint64_t> waitSemaphoresValues;

	Core::RefCountPtr<Swapchain> swapchain;
	uint32_t imageIndex;
};

class Device : public virtual Core::IResource
{
public:
	virtual ~Device() = default;

	//----------- Queue / Command --------------//
	KENGINE_API virtual Core::RefCountPtr<CommandList> GetCommandList(QueueType queueType) = 0;
	KENGINE_API virtual void SubmitCommandList(Core::RefCountPtr<CommandList> commandList, const SubmitInfo& submitInfo) = 0;
	KENGINE_API virtual void WaitIdle() = 0;
	KENGINE_API virtual void QueueWaitIdle(QueueType queueType) = 0;
	KENGINE_API virtual void RunGarbageCollector() = 0;


	//----------- Syncronisation --------------// 
	KENGINE_API virtual Core::RefCountPtr<Semaphore> CreateSemaphore(const SemaphoreSpecs& specs) = 0;
	KENGINE_API virtual void DestroySemaphore(Core::RefCountPtr<Semaphore> semaphore) = 0;
	KENGINE_API virtual void WaitForSemaphore(Core::RefCountPtr<Semaphore> semaphore, uint64_t waitValue) = 0;
	KENGINE_API virtual Core::RefCountPtr<Fence> CreateFence() = 0;
	KENGINE_API virtual void DestroyFence(Core::RefCountPtr<Fence> fence) = 0;
	KENGINE_API virtual void WaitForFence(Core::RefCountPtr<Fence> fence) = 0;
	KENGINE_API virtual void ResetFence(Core::RefCountPtr<Fence> fence) = 0;
	

	//----------- Swapchain --------------// 
	KENGINE_API virtual Core::RefCountPtr<Swapchain> CreateSwapchain(const SwapchainSpecs& specs) = 0;
	KENGINE_API virtual void DestroySwapchain(Core::RefCountPtr<Swapchain> swapchain) = 0;
	KENGINE_API virtual uint32_t AcquirreNextImage(Core::RefCountPtr<Swapchain> swapchain, Core::RefCountPtr<Semaphore> Semaphore) = 0;
	KENGINE_API virtual void Present(const PresentInfo& presentInfo) = 0;


	//-------------- Buffer --------------// 
	KENGINE_API virtual Core::RefCountPtr<Buffer> CreateBuffer(const BufferSpecs& specs) = 0;
	KENGINE_API virtual void DestroyBuffer(Core::RefCountPtr<Buffer> buffer) = 0;


	// -------------- Image -------------- // 
	KENGINE_API virtual Core::RefCountPtr<Image> CreateImage(const ImageSpecs& specs) = 0;
	KENGINE_API virtual void DestroyImage(Core::RefCountPtr<Image> image) = 0;

	KENGINE_API virtual Core::RefCountPtr<Shader> CreateShader(const std::string& file, const ShaderStage& sStage) = 0;
	KENGINE_API virtual void DestroyShader(Core::RefCountPtr<Shader> shader) = 0;

	KENGINE_API virtual Core::RefCountPtr<Pipeline> CreatePipeline(const PipelineSpecs& specs) = 0;
	KENGINE_API virtual void DestroyPipeline(Core::RefCountPtr<Pipeline> pipeline) = 0;

protected:
	ShaderCompiler m_shaderCompiler;
	KENGINE_API virtual std::vector<Core::RefCountPtr<Image>> CreatePresentationImages(Core::RefCountPtr<Swapchain> swapchain) = 0;
	KENGINE_API virtual void DestroyPresentationImages(std::vector<Core::RefCountPtr<Image>> presentationImages) = 0;
	KENGINE_API virtual Core::RefCountPtr<Image> CreateImagesWithSwapchain(const SwapchainImageSpecs& specs, Core::RefCountPtr<Swapchain> swapchain) = 0; // Use classic Destroy
};

END_NAMESPACE_RHI