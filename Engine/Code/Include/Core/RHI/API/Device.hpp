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
	KAPI virtual CommandListHandle GetCommandList(QueueType queueType) = 0;
	KAPI virtual void SubmitCommandList(CommandListHandle commandList, const SubmitInfo& submitInfo) = 0;
	KAPI virtual void WaitIdle() = 0;
	KAPI virtual void QueueWaitIdle(QueueType queueType) = 0;
	KAPI virtual void RunGarbageCollector() = 0;


	//----------- Syncronisation --------------// 
	KAPI virtual SemaphoreHandle CreateSemaphore(const SemaphoreSpecs& specs) = 0;
	KAPI virtual void DestroySemaphore(SemaphoreHandle semaphore) = 0;
	KAPI virtual void WaitForSemaphore(SemaphoreHandle semaphore, uint64_t waitValue) = 0;
	KAPI virtual FenceHandle CreateFence() = 0;
	KAPI virtual void DestroyFence(FenceHandle fence) = 0;
	KAPI virtual void WaitForFence(FenceHandle fence) = 0;
	KAPI virtual void ResetFence(FenceHandle fence) = 0;
	

	//----------- Swapchain --------------// 
	KAPI virtual SwapchainHandle CreateSwapchain(const SwapchainSpecs& specs) = 0;
	KAPI virtual void DestroySwapchain(SwapchainHandle swapchain) = 0;
	KAPI virtual uint32_t AcquirreNextImage(SwapchainHandle swapchain, SemaphoreHandle Semaphore) = 0;
	KAPI virtual bool Present(const PresentInfo& presentInfo) = 0;


	//-------------- Buffer --------------// 
	KAPI virtual BufferHandle CreateBuffer(const BufferSpecs& specs) = 0;
	KAPI virtual void DestroyBuffer(BufferHandle buffer) = 0;


	// -------------- Image -------------- // 
	KAPI virtual ImageHandle CreateImage(const ImageSpecs& specs) = 0;
	KAPI virtual void DestroyImage(ImageHandle image) = 0;
	KAPI virtual std::vector<ImageHandle> CreatePresentationImages(SwapchainHandle swapchain) = 0;
	KAPI virtual void DestroyPresentationImages(std::vector<ImageHandle> presentationImages) = 0;
	KAPI virtual ImageHandle CreateImagesWithSwapchain(const SwapchainImageSpecs& specs, SwapchainHandle swapchain) = 0; // Use classic Destroy

	// -------------- Sampler -------------- // 
	KAPI virtual SamplerHandle CreateSampler(const SamplerSpecs& specs) = 0;
	KAPI virtual void DestroySampler(SamplerHandle sampler) = 0;

	// -------------- Shader -------------- // 
	KAPI virtual ShaderHandle CreateShader(const std::string& file, const ShaderStage& sStage, bool isGlobalLayout, bool usesGlobalLayout) = 0;
	KAPI virtual void DestroyShader(ShaderHandle shader) = 0;

	// -------------- DescriptorSetLayout -------------- // 
	KAPI virtual std::vector<DescriptorSetLayoutHandle> CreateDescriptorSetsLayouts(ShaderHandle shader) = 0;
	KAPI virtual DescriptorSetLayoutHandle CreateDescriptorSetLayout(ShaderHandle shader, std::string name) = 0;
	KAPI virtual DescriptorSetLayoutHandle CreateDescriptorSetLayout(ShaderHandle shader, uint32_t index) = 0;
	KAPI virtual void DestroyDescriptorSetsLayouts(std::vector<DescriptorSetLayoutHandle> descriptors) = 0;
	KAPI virtual void DestroyDescriptorSetLayout(DescriptorSetLayoutHandle descriptors) = 0;

	// -------------- PushConstantLayout -------------- // 
	KAPI virtual PushConstantLayoutHandle CreatePushConstantLayout(ShaderHandle shader) = 0;
	KAPI virtual void DestroyPushConstantsLayouts(std::vector<PushConstantLayoutHandle>& pushConstants) = 0;

	// -------------- Descriptor Set -------------- // 
	KAPI virtual DescriptorSetHandle CreateDescriptorSet(DescriptorSetLayoutHandle layout) = 0;
	KAPI virtual void DestroyDescriptorSet(DescriptorSetHandle descriptorSet) = 0;
	KAPI virtual void SetDescriptorSetBuffer(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, BufferHandle buffer, uint32_t offset, uint32_t range) = 0;
	KAPI virtual void SetDescriptorSetBuffer(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, BufferHandle buffer, uint32_t offset, uint32_t range) = 0;
	KAPI virtual void SetDescriptorSetImage(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, ImageHandle image, SamplerHandle sampler) = 0;
	KAPI virtual void SetDescriptorSetImage(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, ImageHandle image, SamplerHandle sampler) = 0;
	KAPI virtual void SetDescriptorSetSampler(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, SamplerHandle sampler) = 0;
	KAPI virtual void SetDescriptorSetSampler(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, SamplerHandle sampler) = 0;

	// -------------- Pipeline Layout -------------- // 
	KAPI virtual PipelineLayoutHandle CreatePipelineLayout(std::vector<DescriptorSetLayoutHandle> descriptors, std::vector<PushConstantLayoutHandle> pushConstants) = 0;
	KAPI virtual void DestroyPipelineLayout(PipelineLayoutHandle) = 0;

	// -------------- Pipeline -------------- // 
	KAPI virtual GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) = 0;
	// TODO : Implment compute pipeline
	//KAPI virtual Core::RefCountPtr<ComputePipeline> CreateGraphicsPipeline(const ComputePipelineSpecs& specs) = 0;
	KAPI virtual void DestroyPipeline(PipelineHandle pipeline) = 0;

	KAPI virtual void UpdateCompatibility(SurfaceHandle surface) = 0;


protected:
	ShaderCompiler m_shaderCompiler;
	std::unordered_map<std::string, ShaderHandle> m_compiledShaders{};
};

END_NAMESPACE_CORE