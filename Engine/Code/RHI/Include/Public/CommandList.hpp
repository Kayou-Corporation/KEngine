#pragma once

#include "Public/RHI.hpp"

#include <vector>

BEGIN_NAMESPACE_RHI

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

	//----------- Bind --------------//
	KENGINE_API virtual void BindDescriptorSet(Core::RefCountPtr<PipelineLayout> pipelineLayout, std::string layoutName, DescriptorSetHandle descriptorSet, PipelineBindPoint bindPoint) = 0;
	KENGINE_API virtual void BindDescriptorSet(Core::RefCountPtr<PipelineLayout> pipelineLayout, uint32_t layoutIndex, DescriptorSetHandle descriptorSet, PipelineBindPoint bindPoint) = 0;
	KENGINE_API virtual void BindVertexBuffer(BufferHandle vertexBuffer, uint32_t offset) = 0;
	KENGINE_API virtual void BindIndexBuffer(BufferHandle indexBuffer, uint32_t offset) = 0;

	//----------- Draw --------------//
	KENGINE_API virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	KENGINE_API virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	KENGINE_API virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;

	//----------- Set Buffer / Image Data --------------//
	KENGINE_API virtual void SetBufferData(BufferHandle buffer, void* data, uint32_t size, uint32_t offset) = 0;
	KENGINE_API virtual void CopyBufferToBuffer(BufferHandle srcBuffer, uint32_t srcOffset, BufferHandle dstBuffer, uint32_t dstOffset, uint32_t size, bool returnSrcBufferToInitialStage = true, bool returnDstBufferToInitialStage = true) = 0;
	KENGINE_API virtual void SetImageData(Core::RefCountPtr<Image> image, void* data, uint32_t size) = 0;
	// TODO : Implement Mips properly
	KENGINE_API virtual void CopyImageToImage(Core::RefCountPtr<Image> srcImage, Extent3D srcOffset, Core::RefCountPtr<Image> dstImage, Extent3D dstOffset, bool returnSrcImageToInitialStage = true, bool returnDstImageToInitialStage = true) = 0;
	//KENGINE_API virtual void CopyImageToBuffer(Core::RefCountPtr<Image> srcImage, BufferHandle dstBuffer);	
	//KENGINE_API virtual void CopyBufferToImage(BufferHandle srcBuffer, Core::RefCountPtr<Image> dstImage);


	//----------- Transition Image Layout --------------//
	KENGINE_API virtual void TransitionImageLayout(Core::RefCountPtr<Image> image, Layout dstLayout) = 0;

	//----------- Pipeline --------------//
	KENGINE_API virtual void BindPipeline(Core::RefCountPtr<Pipeline> RHIPipeline) = 0;
};

END_NAMESPACE_RHI