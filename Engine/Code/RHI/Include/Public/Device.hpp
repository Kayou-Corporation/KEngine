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

	KENGINE_API virtual void WaitIdle() = 0;
	KENGINE_API virtual void QueueWaitIdle(QueueType type) = 0;


	KENGINE_API virtual Core::RefCountPtr<Swapchain> CreateSwapchain(const SwapchainSpecs& specs) = 0;
	KENGINE_API virtual void DestroySwapchain(Core::RefCountPtr<Swapchain> buffer) = 0;

	KENGINE_API virtual Core::RefCountPtr<Buffer> CreateBuffer(const BufferSpecs& specs) = 0;
	KENGINE_API virtual void DestroyBuffer(Core::RefCountPtr<Buffer> buffer) = 0;

	KENGINE_API virtual Core::RefCountPtr<Image> CreateImage(const ImageSpecs& specs) = 0;
	KENGINE_API virtual void DestroyImage(Core::RefCountPtr<Image> image) = 0;

	KENGINE_API virtual Core::RefCountPtr<Shader> CreateShader(const std::string& file, const ShaderType& sType) = 0;
	KENGINE_API virtual void DestroyShader(Core::RefCountPtr<Shader> shader) = 0;

protected:
	ShaderCompiler m_shaderCompiler;
};

END_NAMESPACE_RHI