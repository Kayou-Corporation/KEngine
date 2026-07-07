#pragma once

#include <vector>
#include <unordered_map>

#include "RHI/API/RHI.hpp"
#include "RHI/API/Shader.hpp"

BEGIN_NAMESPACE_CORE

struct DeviceSpecs
{
	GpuType gpuType;
	std::vector<Features> features; // Not implemented 

	std::vector<Extensions> extensions;
	std::vector<QueueType> queues;
	bool searchPresentQueue;
	SurfaceHandle surface;
};

struct PresentInfo
{
	std::vector<SemaphoreHandle> waitSemaphores;
	std::vector<uint64_t> waitSemaphoresValues;

	SwapchainHandle swapchain;
	uint32_t imageIndex;
};

class Device : public virtual Core::IResource
{
public:
	virtual ~Device() = default;

	//----------- Queue / Command --------------//
	KENGINE_API virtual CommandListHandle GetCommandList(QueueType queueType) = 0;
	KENGINE_API virtual void SubmitCommandList(CommandListHandle commandList, const SubmitInfo& submitInfo) = 0;
	KENGINE_API virtual void WaitIdle() = 0;
	KENGINE_API virtual void QueueWaitIdle(QueueType queueType) = 0;
	KENGINE_API virtual void RunGarbageCollector() = 0;


	//----------- Syncronisation --------------// 
	KENGINE_API virtual SemaphoreHandle CreateSemaphore(const SemaphoreSpecs& specs) = 0;
	KENGINE_API virtual void DestroySemaphore(SemaphoreHandle semaphore) = 0;
	KENGINE_API virtual void WaitForSemaphore(SemaphoreHandle semaphore, uint64_t waitValue) = 0;
	KENGINE_API virtual FenceHandle CreateFence() = 0;
	KENGINE_API virtual void DestroyFence(FenceHandle fence) = 0;
	KENGINE_API virtual void WaitForFence(FenceHandle fence) = 0;
	KENGINE_API virtual void ResetFence(FenceHandle fence) = 0;
	

	//----------- Swapchain --------------// 
	KENGINE_API virtual SwapchainHandle CreateSwapchain(const SwapchainSpecs& specs) = 0;
	KENGINE_API virtual void DestroySwapchain(SwapchainHandle swapchain) = 0;
	KENGINE_API virtual uint32_t AcquirreNextImage(SwapchainHandle swapchain, SemaphoreHandle Semaphore) = 0;
	KENGINE_API virtual bool Present(const PresentInfo& presentInfo) = 0;


	//-------------- Buffer --------------// 
	KENGINE_API virtual BufferHandle CreateBuffer(const BufferSpecs& specs) = 0;
	KENGINE_API virtual void DestroyBuffer(BufferHandle buffer) = 0;


	// -------------- Image -------------- // 
	KENGINE_API virtual ImageHandle CreateImage(const ImageSpecs& specs) = 0;
	KENGINE_API virtual void DestroyImage(ImageHandle image) = 0;
	KENGINE_API virtual std::vector<ImageHandle> CreatePresentationImages(SwapchainHandle swapchain) = 0;
	KENGINE_API virtual void DestroyPresentationImages(std::vector<ImageHandle> presentationImages) = 0;
	KENGINE_API virtual ImageHandle CreateImagesWithSwapchain(const SwapchainImageSpecs& specs, SwapchainHandle swapchain) = 0; // Use classic Destroy

	// -------------- Sampler -------------- // 
	KENGINE_API virtual SamplerHandle CreateSampler(const SamplerSpecs& specs) = 0;
	KENGINE_API virtual void DestroySampler(SamplerHandle sampler) = 0;

	// -------------- Shader -------------- // 
	KENGINE_API virtual ShaderHandle CreateShader(const std::string& file, const ShaderStage& sStage, bool isGlobalLayout, bool usesGlobalLayout) = 0;
	KENGINE_API virtual void DestroyShader(ShaderHandle shader) = 0;

	// -------------- DescriptorSetLayout -------------- // 
	KENGINE_API virtual std::vector<DescriptorSetLayoutHandle> CreateDescriptorSetsLayouts(ShaderHandle shader) = 0;
	KENGINE_API virtual DescriptorSetLayoutHandle CreateDescriptorSetLayout(ShaderHandle shader, std::string name) = 0;
	KENGINE_API virtual DescriptorSetLayoutHandle CreateDescriptorSetLayout(ShaderHandle shader, uint32_t index) = 0;
	KENGINE_API virtual void DestroyDescriptorSetsLayouts(std::vector<DescriptorSetLayoutHandle> descriptors) = 0;
	KENGINE_API virtual void DestroyDescriptorSetLayout(DescriptorSetLayoutHandle descriptors) = 0;

	// -------------- PushConstantLayout -------------- // 
	KENGINE_API virtual PushConstantLayoutHandle CreatePushConstantLayout(ShaderHandle shader) = 0;
	KENGINE_API virtual void DestroyPushConstantsLayouts(std::vector<PushConstantLayoutHandle>& pushConstants) = 0;

	// -------------- Descriptor Set -------------- // 
	KENGINE_API virtual DescriptorSetHandle CreateDescriptorSet(DescriptorSetLayoutHandle layout) = 0;
	KENGINE_API virtual void DestroyDescriptorSet(DescriptorSetHandle descriptorSet) = 0;
	KENGINE_API virtual void SetDescriptorSetBuffer(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, BufferHandle buffer, uint32_t offset, uint32_t range) = 0;
	KENGINE_API virtual void SetDescriptorSetBuffer(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, BufferHandle buffer, uint32_t offset, uint32_t range) = 0;
	KENGINE_API virtual void SetDescriptorSetImage(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, ImageHandle image, SamplerHandle sampler) = 0;
	KENGINE_API virtual void SetDescriptorSetImage(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, ImageHandle image, SamplerHandle sampler) = 0;
	KENGINE_API virtual void SetDescriptorSetSampler(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, SamplerHandle sampler) = 0;
	KENGINE_API virtual void SetDescriptorSetSampler(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, SamplerHandle sampler) = 0;

	// -------------- Pipeline Layout -------------- // 
	KENGINE_API virtual PipelineLayoutHandle CreatePipelineLayout(std::vector<DescriptorSetLayoutHandle> descriptors, std::vector<PushConstantLayoutHandle> pushConstants) = 0;
	KENGINE_API virtual void DestroyPipelineLayout(PipelineLayoutHandle) = 0;

	// -------------- Pipeline -------------- // 
	KENGINE_API virtual GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) = 0;
	// TODO : Implment compute pipeline
	//KENGINE_API virtual Core::RefCountPtr<ComputePipeline> CreateGraphicsPipeline(const ComputePipelineSpecs& specs) = 0;
	KENGINE_API virtual void DestroyPipeline(PipelineHandle pipeline) = 0;

	KENGINE_API virtual void UpdateCompatibility(SurfaceHandle surface) = 0;


protected:
	ShaderCompiler m_shaderCompiler;
	std::unordered_map<std::string, ShaderHandle> m_compiledShaders{};
};

END_NAMESPACE_CORE