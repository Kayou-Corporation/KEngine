#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

#include <vector>

BEGIN_NAMESPACE_RHI

class Buffer;
class Image;
class Device;
struct RenderingInfo;

class Fence;
class Semaphore;

class Pipeline;

struct SubmitInfo
{
	SubmitInfo() = default;

	PipelineStage stage;

	std::vector<Core::RefCountPtr<Semaphore>> signalSemaphores;
	std::vector<uint64_t> signalSemaphoresValues;
	std::vector<Core::RefCountPtr<Semaphore>> waitSemaphores;
	std::vector<uint64_t> waitSemaphoresValues;

	// Deprecated
	//Core::RefCountPtr<Fence> fence;
};

class CommandList : virtual public Core::IResource
{
public:
	virtual ~CommandList() = default;

	//----------- Open / Close --------------//
	KENGINE_API virtual void Open() = 0;
	KENGINE_API virtual void Close() = 0;


	//----------- Dynamic Rendering --------------//
	KENGINE_API virtual void BeginRendering(const RenderingInfo& renderingInfo) = 0;
	KENGINE_API virtual void EndRendering() = 0;


	//----------- Set Buffer / Image Data --------------//
	KENGINE_API virtual void SetBufferData(Core::RefCountPtr<Buffer> buffer, void* data, uint32_t size, uint32_t offset) = 0;
	KENGINE_API virtual void SetImageData(Core::RefCountPtr<Image> image, void* data, uint32_t size) = 0;


	//----------- Transition Image Layout --------------//
	KENGINE_API virtual void TransitionImageLayout(Core::RefCountPtr<Image> image, Layout dstLayout) = 0;

	//----------- Pipeline --------------//
	KENGINE_API virtual void BindPipeline(Core::RefCountPtr<Pipeline> RHIPipeline) = 0;
};

END_NAMESPACE_RHI