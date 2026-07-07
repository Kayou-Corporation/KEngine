#pragma once

#include "RHI/API/RHI.hpp"

#include <vector>

BEGIN_NAMESPACE_CORE

struct SubmitInfo
{
	SubmitInfo() = default;

	PipelineStage stage;

	std::vector<SemaphoreHandle> signalSemaphores;
	std::vector<uint64_t> signalSemaphoresValues;
	std::vector<SemaphoreHandle> waitSemaphores;
	std::vector<uint64_t> waitSemaphoresValues;

	// Deprecated
	//FenceHandle fence;
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
	KENGINE_API virtual void BindDescriptorSet(PipelineLayoutHandle pipelineLayout, std::string layoutName, DescriptorSetHandle descriptorSet, PipelineBindPoint bindPoint) = 0;
	KENGINE_API virtual void BindDescriptorSet(PipelineLayoutHandle pipelineLayout, uint32_t layoutIndex, DescriptorSetHandle descriptorSet, PipelineBindPoint bindPoint) = 0;
	KENGINE_API virtual void BindVertexBuffer(BufferHandle vertexBuffer, uint32_t offset) = 0;
	KENGINE_API virtual void BindIndexBuffer(BufferHandle indexBuffer, uint32_t offset) = 0;

	//----------- Draw --------------//
	KENGINE_API virtual void SetViewport(float x, float y, float width, float height) = 0;
	KENGINE_API virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	KENGINE_API virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;

	//----------- Set Buffer / Image Data --------------//
	KENGINE_API virtual void SetBufferData(BufferHandle buffer, void* data, uint32_t size, uint32_t offset) = 0;
	KENGINE_API virtual void CopyBufferToBuffer(BufferHandle srcBuffer, uint32_t srcOffset, BufferHandle dstBuffer, uint32_t dstOffset, uint32_t size, bool returnSrcBufferToInitialStage = true, bool returnDstBufferToInitialStage = true) = 0;
	KENGINE_API virtual void SetImageData(ImageHandle image, void* data, uint32_t size) = 0;
	// TODO : Implement Mips properly
	KENGINE_API virtual void CopyImageToImage(ImageHandle srcImage, Extent3D srcOffset, ImageHandle dstImage, Extent3D dstOffset, bool returnSrcImageToInitialStage = true, bool returnDstImageToInitialStage = true) = 0;
	//KENGINE_API virtual void CopyImageToBuffer(ImageHandle srcImage, BufferHandle dstBuffer);	
	//KENGINE_API virtual void CopyBufferToImage(BufferHandle srcBuffer, ImageHandle dstImage);


	//----------- Transition Image Layout --------------//
	KENGINE_API virtual void TransitionImageLayout(ImageHandle image, Layout dstLayout) = 0;

	//----------- Pipeline --------------//
	KENGINE_API virtual void BindPipeline(PipelineHandle RHIPipeline) = 0;
};

END_NAMESPACE_CORE