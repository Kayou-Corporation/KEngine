#include "Private/Vulkan/VulkanDevice.hpp"

#include "Private/Vulkan/VulkanSurface.hpp"
#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanTranslate.hpp"
#include "Private/Vulkan/VulkanSwapchain.hpp"
#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanImage.hpp"
#include "Private/Vulkan/VulkanShader.hpp"
#include "Private/Vulkan/VulkanGraphicsPipeline.hpp"
#include "Private/Vulkan/VulkanCommandList.hpp"
#include "Private/Vulkan/VulkanSyncronisation.hpp"

#include <map>
#include <set>
#include <string>
#include <spdlog/spdlog.h>

// TODO : Maybe do more cleanup (Review extension system and constant validation for format usage)

DISABLE_WARNINGS

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_RHI


// PUBLIC : 

VulkanDevice::VulkanDevice()
{
	m_shaderCompiler.Initialize();
	m_queueFamily = QueueFamily();
	m_memoryAllocator = nullptr;
}

//----------- Queue / Command --------------//
Core::RefCountPtr<CommandList> VulkanDevice::GetCommandList(QueueType RHIQueueType)
{
	Core::RefCountPtr<VulkanCommandList> RHIVulkanCommandList = Core::CreateRefPtr<VulkanCommandList>();

	TrackedCommandBufferPtr commandBuffer = m_queues[RHIQueueType].GetOrCreateCommandBuffer(m_handle);

	RHIVulkanCommandList->SetHandle(commandBuffer);
	RHIVulkanCommandList->SetOwnerQueueType(RHIQueueType);

	return RHIVulkanCommandList;
}

void VulkanDevice::SubmitCommandList(Core::RefCountPtr<CommandList> RHICommandList, const SubmitInfo& RHISubmitInfo)
{
	Core::RefCountPtr<VulkanCommandList> RHIVulkanCommandList = RHICommandList.CastAs<VulkanCommandList>();
	
	TrackedCommandBufferPtr commandBuffer = RHIVulkanCommandList->GetHandle();
	QueueType RHICommandBufferQueue = RHIVulkanCommandList->GetOwnerQueueType();

	std::vector<vk::Semaphore> signalSemaphores;
	for (uint32_t i = 0; i < RHISubmitInfo.signalSemaphores.size(); ++i)
	{
		Core::RefCountPtr<VulkanSemaphore> RHIVulkanSemaphore = RHISubmitInfo.signalSemaphores[i].CastAs<VulkanSemaphore>();

		vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

		signalSemaphores.push_back(semaphore);
	}
	m_queues[RHICommandBufferQueue].PushSignalSemaphores(signalSemaphores, RHISubmitInfo.signalSemaphoresValues);

	std::vector<vk::Semaphore> waitSemaphores;
	for (uint32_t i = 0; i < RHISubmitInfo.waitSemaphores.size(); ++i)
	{
		Core::RefCountPtr<VulkanSemaphore> RHIVulkanSemaphore = RHISubmitInfo.waitSemaphores[i].CastAs<VulkanSemaphore>();

		vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

		waitSemaphores.push_back(semaphore);
	}
	m_queues[RHICommandBufferQueue].PushWaitSemaphores(waitSemaphores, RHISubmitInfo.waitSemaphoresValues);

	vk::PipelineStageFlagBits stage = TranslateToVulkan(RHISubmitInfo.stage);

	m_queues[RHICommandBufferQueue].Submit(commandBuffer, stage);
}

void VulkanDevice::WaitIdle()
{
	VK_CHECK_VOID(m_handle.waitIdle(), "Device can't wait idle");
}

void VulkanDevice::QueueWaitIdle(QueueType queueType)
{
	m_queues[queueType].WaitIdle();
}

void VulkanDevice::RunGarbageCollector()
{
	for (auto& [type, queue] : m_queues)
	{
		queue.RunGarbageCollector(m_handle);
	}
}

//----------- Syncronisation --------------// 
Core::RefCountPtr<Semaphore> VulkanDevice::CreateSemaphore(const SemaphoreSpecs& RHISpecs)
{
	Core::RefCountPtr<VulkanSemaphore> RHIVulkanSemaphore = Core::CreateRefPtr<VulkanSemaphore>();

	vk::SemaphoreCreateInfo createInfo{};
	vk::SemaphoreTypeCreateInfo typeInfo{};

	if (RHISpecs.type == SemaphoreType::Timeline)
	{
		typeInfo.semaphoreType = vk::SemaphoreType::eTimeline;
		typeInfo.initialValue = RHISpecs.timelineValue;
		createInfo.pNext = &typeInfo;
	}

	vk::Semaphore semaphore = VK_CHECK_RESULT(m_handle.createSemaphore(createInfo), "Coudn't create semaphore");

	RHIVulkanSemaphore->SetHandle(semaphore);

	return RHIVulkanSemaphore;
}

void VulkanDevice::DestroySemaphore(Core::RefCountPtr<Semaphore> RHISemaphore)
{
	Core::RefCountPtr<VulkanSemaphore> RHIVulkanSemaphore = RHISemaphore.CastAs<VulkanSemaphore>();

	vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandleRef();

	m_handle.destroySemaphore(semaphore);
}

void VulkanDevice::WaitForSemaphore(Core::RefCountPtr<Semaphore> RHISemaphore, uint64_t waitValue)
{
	Core::RefCountPtr<VulkanSemaphore> RHIVulkanSemaphore = RHISemaphore.CastAs<VulkanSemaphore>();

	vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

	vk::SemaphoreWaitInfo waitInfo{};
	waitInfo.flags = vk::SemaphoreWaitFlagBits::eAny;
	waitInfo.semaphoreCount = 1;
	waitInfo.pSemaphores = &semaphore;
	waitInfo.pValues = &waitValue;

	VK_CHECK_VOID(m_handle.waitSemaphores(waitInfo, UINT64_MAX), "Can't wait semaphore");
}

Core::RefCountPtr<Fence> VulkanDevice::CreateFence()
{
	Core::RefCountPtr<VulkanFence> RHIVulkanFence = Core::CreateRefPtr<VulkanFence>();

	vk::FenceCreateInfo createInfo{};
	createInfo.flags = vk::FenceCreateFlagBits::eSignaled;
	vk::Fence fence = VK_CHECK_RESULT(m_handle.createFence(createInfo), "Coudn't create fence");

	RHIVulkanFence->SetHandle(fence);

	return RHIVulkanFence;
}

void VulkanDevice::DestroyFence(Core::RefCountPtr<Fence> RHIFence)
{
	Core::RefCountPtr<VulkanFence> RHIVulkanFence = RHIFence.CastAs<VulkanFence>();

	vk::Fence fence = RHIVulkanFence->GetHandleRef();

	m_handle.destroyFence(fence);
}

void VulkanDevice::WaitForFence(Core::RefCountPtr<Fence> RHIFence)
{
	Core::RefCountPtr<VulkanFence> RHIVulkanFence = RHIFence.CastAs<VulkanFence>();

	vk::Fence fence = RHIVulkanFence->GetHandle();

	VK_CHECK_VOID(m_handle.waitForFences({ fence }, VK_TRUE, UINT64_MAX), "can't wait for fence");
}

void VulkanDevice::ResetFence(Core::RefCountPtr<Fence> RHIFence)
{
	Core::RefCountPtr<VulkanFence> RHIVulkanFence = RHIFence.CastAs<VulkanFence>();

	vk::Fence fence = RHIVulkanFence->GetHandle();

	VK_CHECK_VOID(m_handle.resetFences({ fence }), "Can't reset for fence");
}

//----------- Swapchain --------------// 
// TODO (Eliott) : Do some cleanup on GetCreateInfo
Core::RefCountPtr<Swapchain> VulkanDevice::CreateSwapchain(const SwapchainSpecs& RHISpecs)
{
	Core::RefCountPtr<VulkanSwapchain> RHIVulkanSwapchain = Core::CreateRefPtr<VulkanSwapchain>();

	std::unordered_map<int, int> map;

	vk::SurfaceKHR& surface = RHISpecs.surface.CastAs<VulkanSurface>()->GetHandleRef();

	uint32_t graphicsQueueIndex = m_queueFamily.GetQueues().at(QueueType::Graphics).value();
	uint32_t presentQueueIndex = m_queueFamily.GetPresentQueueIndex();

	vk::Format requestedFormat = TranslateToVulkan(RHISpecs.imageFormat);
	vk::Format requestedDepthFormat = TranslateToVulkan(RHISpecs.depthImageFormat);

	vk::Format depthFormat = CheckFormatCompatibility(requestedDepthFormat, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);

	vk::PresentModeKHR requestedpresentMode = TranslateToVulkan(RHISpecs.presentMode);

	vk::Extent2D requestedExtent = TranslateToVulkan(RHISpecs.extent);

	vk::SwapchainCreateInfoKHR createInfo = RHIVulkanSwapchain->GetCreateInfo(m_compatibility, surface, graphicsQueueIndex, presentQueueIndex, RHISpecs.imageCount, requestedFormat, RHISpecs.isDepthEnable, depthFormat, requestedpresentMode, requestedExtent);

	vk::SwapchainKHR swapchain = VK_CHECK_RESULT(m_handle.createSwapchainKHR(createInfo), "Can't create swapchain");

	RHIVulkanSwapchain->SetHandle(swapchain);

	return RHIVulkanSwapchain;
}

void VulkanDevice::DestroySwapchain(Core::RefCountPtr<Swapchain> RHISwapchain)
{
	Core::RefCountPtr<VulkanSwapchain> RHIVulkanSwapchain = RHISwapchain.CastAs<VulkanSwapchain>();

	m_handle.destroySwapchainKHR(RHIVulkanSwapchain->GetHandle());
}

uint32_t VulkanDevice::AcquirreNextImage(Core::RefCountPtr<Swapchain> RHISwapchain, Core::RefCountPtr<Semaphore> RHISemaphore)
{
	Core::RefCountPtr<VulkanSwapchain> RHIVulkanSwapchain = RHISwapchain.CastAs<VulkanSwapchain>();
	Core::RefCountPtr<VulkanSemaphore> RHIVulkanSemaphore = RHISemaphore.CastAs<VulkanSemaphore>();

	vk::SwapchainKHR swapchain = RHIVulkanSwapchain->GetHandle();
	vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

	uint32_t imageIndex = VK_CHECK_RESULT(m_handle.acquireNextImageKHR(swapchain, UINT64_MAX, semaphore), "Failed to acquired next image");

	return imageIndex;
}

void VulkanDevice::Present(const PresentInfo& RHIPresentInfo)
{
	std::vector<vk::Semaphore> waitSemaphores;
	for (uint32_t i = 0; i < RHIPresentInfo.waitSemaphores.size(); ++i)
	{
		Core::RefCountPtr<VulkanSemaphore> RHIVulkanSemaphore = RHIPresentInfo.waitSemaphores[i].CastAs<VulkanSemaphore>();

		vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

		waitSemaphores.push_back(semaphore);
	}

	Core::RefCountPtr<VulkanSwapchain> RHIVulkanSwapchain = RHIPresentInfo.swapchain.CastAs<VulkanSwapchain>();
	vk::SwapchainKHR swapchain = RHIVulkanSwapchain->GetHandle();

	vk::PresentInfoKHR presentInfo;
	presentInfo.waitSemaphoreCount = waitSemaphores.size();
	presentInfo.pWaitSemaphores = waitSemaphores.data();
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &RHIPresentInfo.imageIndex;

	VK_CHECK_VOID(m_presentQueue.presentKHR(presentInfo), "Can't present");
}

//-------------- Buffer --------------// 
Core::RefCountPtr<Buffer> VulkanDevice::CreateBuffer(const BufferSpecs& RHISpecs)
{
	Core::RefCountPtr<VulkanBuffer> RHIVulkanBuffer = Core::CreateRefPtr<VulkanBuffer>();

	VkBufferCreateInfo bufferInfo = RHIVulkanBuffer->GetCreateInfo(RHISpecs);

	VmaAllocationCreateInfo allocInfo = TranslateToVulkan(RHISpecs.memoryAccess);
	if (RHIVulkanBuffer->GetIsPersistentMapped())
		allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VkBuffer buf;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;
	VK_CHECK_VOID(static_cast<vk::Result>(vmaCreateBuffer(m_memoryAllocator, &bufferInfo, &allocInfo, &buf, &allocation, &allocationInfo)), "Failed to create buffer");

	RHIVulkanBuffer->SetHandle(static_cast<vk::Buffer>(buf));
	RHIVulkanBuffer->SetAllocation(allocation);
	RHIVulkanBuffer->SetAllocationInfo(allocationInfo);

	if (RHISpecs.memoryAccess == MemoryAccess::GPU_Only)
		RHIVulkanBuffer->SetIsGpuOnly(true);

	return RHIVulkanBuffer;
}

void VulkanDevice::DestroyBuffer(Core::RefCountPtr<Buffer> RHIBuffer)
{
	Core::RefCountPtr<VulkanBuffer> RHIVulkanBuffer = RHIBuffer.CastAs<VulkanBuffer>();

	VkBuffer rawBuffer = static_cast<VkBuffer>(RHIVulkanBuffer->GetHandle());
	VmaAllocation bufferAllocation = RHIVulkanBuffer->GetAllocation();

	vmaDestroyBuffer(m_memoryAllocator, rawBuffer, bufferAllocation);
}

//-------------- Image --------------// 
Core::RefCountPtr<Image> VulkanDevice::CreateImage(const ImageSpecs& RHISpecs)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = Core::CreateRefPtr<VulkanImage>();

	VkImageCreateInfo imageCreateInfo = static_cast<VkImageCreateInfo>(RHIVulkanImage->GetCreateInfo(RHISpecs));

	VkImage image;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = 0;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;

	VK_CHECK_VOID(static_cast<vk::Result>(vmaCreateImage(m_memoryAllocator, &imageCreateInfo, &allocInfo, &image, &allocation, &allocationInfo)), "Failed to create image");

	RHIVulkanImage->SetHandle(image);

	vk::ImageViewCreateInfo imageViewCreateInfo = RHIVulkanImage->GetViewCreateInfo(RHISpecs);
	vk::ImageView imageView = VK_CHECK_RESULT(m_handle.createImageView(imageViewCreateInfo, nullptr), "Failed to create image");

	RHIVulkanImage->SetAllocation(allocation);
	RHIVulkanImage->SetAllocationInfo(allocationInfo);

	RHIVulkanImage->SetHandleView(imageView);

	return RHIVulkanImage;
}

void VulkanDevice::DestroyImage(Core::RefCountPtr<Image> RHIImage)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIImage.CastAs<VulkanImage>();

	vk::Image image = RHIVulkanImage->GetHandleRef();
	vk::ImageView imageView = RHIVulkanImage->GetHandleViewRef();
	VmaAllocation imageAllocation = RHIVulkanImage->GetAllocation();

	m_handle.destroyImageView(imageView);

	vmaDestroyImage(m_memoryAllocator, image, imageAllocation);
}

std::vector<Core::RefCountPtr<Image>> VulkanDevice::CreatePresentationImages(Core::RefCountPtr<Swapchain> RHISwapchain)
{
	Core::RefCountPtr<VulkanSwapchain> RHIVulkanSwapchain = RHISwapchain.CastAs<VulkanSwapchain>();
	uint32_t swapchainImageCount = RHIVulkanSwapchain->GetImageCount();

	std::vector<Core::RefCountPtr<Image>> RHIImages(swapchainImageCount);

	vk::SwapchainKHR swapchain = RHIVulkanSwapchain->GetHandle();
	std::vector<vk::Image> swapchainImages = VK_CHECK_RESULT(m_handle.getSwapchainImagesKHR(swapchain), "Coudn't extract images from swapchain");

	ASSERT(swapchainImageCount == RHIImages.size(), "Swapchain images count don't match acquired images");

	vk::Format colorImageFormat = RHIVulkanSwapchain->GetColorImageFormat();
	vk::Extent2D imageExtent = RHIVulkanSwapchain->GetImageExtent();

	for (uint32_t i = 0; i < RHIImages.size(); ++i)
	{
		Core::RefCountPtr<VulkanImage> RHIVulkanImage = Core::CreateRefPtr<VulkanImage>();

		RHIVulkanImage->SetHandle(swapchainImages[i]);

		vk::ImageViewCreateInfo imageViewCreateInfo = RHIVulkanImage->GetViewCreateInfoForPresentation(colorImageFormat, { imageExtent.width, imageExtent.height, 0 });

		vk::ImageView imageView = VK_CHECK_RESULT(m_handle.createImageView(imageViewCreateInfo, nullptr), "Failed to create image");


		RHIVulkanImage->SetHandleView(imageView);

		RHIImages[i] = RHIVulkanImage;
	}

	return RHIImages;
}

void VulkanDevice::DestroyPresentationImages(std::vector<Core::RefCountPtr<Image>> RHIPresentationImages)
{
	// With swapchain images you only destroy their vk::ImageView and not the vk::Image
	for (uint32_t i = 0; i < RHIPresentationImages.size(); ++i)
	{
		Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIPresentationImages[i].CastAs<VulkanImage>();

		vk::ImageView imageViewHandle = RHIVulkanImage->GetHandleViewRef();

		m_handle.destroyImageView(imageViewHandle);
	}
}

Core::RefCountPtr<Image> VulkanDevice::CreateImagesWithSwapchain(const SwapchainImageSpecs& RHISpecs, Core::RefCountPtr<Swapchain> RHISwapchain)
{
	Core::RefCountPtr<VulkanSwapchain> RHIVulkanSwapchain = RHISwapchain.CastAs<VulkanSwapchain>();
	[[maybe_unused]] uint32_t swapchainImageCount = RHIVulkanSwapchain->GetImageCount();

	Core::RefCountPtr<VulkanImage> RHIVulkanImage = Core::CreateRefPtr<VulkanImage>();

	vk::Format imageFormat;
	if (RHISpecs.imageType == SwapchainImageType::Color)
	{
		imageFormat = RHIVulkanSwapchain->GetColorImageFormat();
	}
	else
	{
		imageFormat = RHIVulkanSwapchain->GetDepthImageFormat();
	}
	vk::Extent2D imageExtent = RHIVulkanSwapchain->GetImageExtent();

	VkImageCreateInfo imageCreateInfo = static_cast<VkImageCreateInfo>(RHIVulkanImage->GetCreateInfoForSwapchain(RHISpecs, imageFormat, { imageExtent.width, imageExtent.height, 1 }));

	VkImage image;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = 0;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;

	VK_CHECK_VOID(static_cast<vk::Result>(vmaCreateImage(m_memoryAllocator, &imageCreateInfo, &allocInfo, &image, &allocation, &allocationInfo)), "Failed to create image");

	RHIVulkanImage->SetHandle(image);

	vk::ImageViewCreateInfo imageViewCreateInfo = RHIVulkanImage->GetViewCreateInfoForSwapchain(RHISpecs);

	vk::ImageView imView = VK_CHECK_RESULT(m_handle.createImageView(imageViewCreateInfo, nullptr), "Failed to create image");

	RHIVulkanImage->SetAllocation(allocation);
	RHIVulkanImage->SetAllocationInfo(allocationInfo);

	RHIVulkanImage->SetHandleView(imView);

	return RHIVulkanImage;
}

//-------------- Shader --------------// 
Core::RefCountPtr<Shader> VulkanDevice::CreateShader(const std::string& file, const ShaderStage& sStage)
{
	Core::RefCountPtr<VulkanShader> shader{};

	if ((shader = m_compiledShaders[file].CastAs<VulkanShader>()))
		return shader;

	shader = Core::CreateRefPtr<VulkanShader>();

	ShaderData bin = m_shaderCompiler.Load(file, sStage);

	size_t size = bin.spirv.size();

	if (size % 4 != 0 || size == 0)
	{
		spdlog::error("SPIR-V size not multiple of 4 for: {}", file);
		return {};
	}

	vk::ShaderModuleCreateInfo createInfo;
	createInfo.codeSize = size;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(bin.spirv.data());

	shader->SetModule(VK_CHECK_RESULT(m_handle.createShaderModule(createInfo), "Failed to create shader module"));
	shader->SetShaderStage(sStage);
	shader->SetDescriptors(bin.descriptors);
	shader->SetVertexAttributes(bin.vertexAttributes);
	shader->SetVertexBindings(bin.vertexBindings);

	m_compiledShaders[file] = shader;

	return shader;
}

void VulkanDevice::DestroyShader(Core::RefCountPtr<Shader> shader)
{
	vk::ShaderModule shaderModule = shader.CastAs<VulkanShader>()->GetModule();

	m_handle.destroyShaderModule(shaderModule);
}

//-------------- Pipeline --------------// 
Core::RefCountPtr<GraphicsPipeline> VulkanDevice::CreateGraphicsPipeline(const GraphicsPipelineSpecs& RHISpecs)
{
	Core::RefCountPtr<VulkanGraphicsPipeline> RHIVulkanPipeline = Core::CreateRefPtr<VulkanGraphicsPipeline>();
	// Get Unique set in shaders
	std::vector<uint32_t> recordedSets = std::vector<uint32_t>(0);
	for (const auto& RHIShader : RHISpecs.shaders)
	{
		for (const auto& RHISet : RHIShader->GetDescriptors())
		{
			if (std::find(recordedSets.begin(), recordedSets.end(), RHISet.index) == recordedSets.end())
			{
				recordedSets.push_back(RHISet.index);
			}
		}
	}

	std::vector<Descriptor> RHIDescriptors;
	for (size_t i = 0; i < recordedSets.size(); ++i)
	{
		uint32_t setIndex = recordedSets[i];
		Descriptor RHIDescriptor{};
		RHIDescriptor.index = setIndex;

		for (const auto& RHIShader : RHISpecs.shaders)
		{
			for (const auto& RHISet : RHIShader->GetDescriptors())
			{
				if (RHISet.index != setIndex)
					continue;

				for (const auto& RHIBinding : RHISet.bindings)
					RHIDescriptor.bindings.push_back(RHIBinding);
			}
		}
		RHIDescriptors.push_back(RHIDescriptor);
	}
	std::sort(RHIDescriptors.begin(), RHIDescriptors.end());
	RHIDescriptors.erase(std::unique(RHIDescriptors.begin(), RHIDescriptors.end()), RHIDescriptors.end());

	std::vector<VulkanDescriptorSetLayoutSpecs> RHIVulkanDescrptorSpecs = RHIVulkanPipeline->GetDescriptorSetLayoutCreateInfo(RHIDescriptors);
	std::vector<vk::DescriptorSetLayoutCreateInfo> descriptorsCreateInfos = RHIVulkanPipeline->GetVulkanDescriptorSetLayoutCreateInfo(RHIVulkanDescrptorSpecs);

	for (const vk::DescriptorSetLayoutCreateInfo& DescriptorSetCreateInfo : descriptorsCreateInfos)
	{
		vk::DescriptorSetLayout descriptor;
		descriptor = VK_CHECK_RESULT(m_handle.createDescriptorSetLayout(DescriptorSetCreateInfo), "Failed to create descriptor set layouyt");
		RHIVulkanPipeline->AddDescriptor(descriptor);
	}

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = RHIVulkanPipeline->GetPipelineLayoutCreateInfo();
	vk::PipelineLayout layout = VK_CHECK_RESULT(m_handle.createPipelineLayout(pipelineLayoutCreateInfo), "Failed to create layout");
	RHIVulkanPipeline->SetLayout(layout);

	VulkanGraphicsPipelineStructs RHIVulkanGraphicsPipelineCreateInfo = RHIVulkanPipeline->GetGraphicsCreateInfo(RHISpecs);
	vk::GraphicsPipelineCreateInfo createInfo = RHIVulkanPipeline->GetVulkanGraphicsCreateInfo(RHIVulkanGraphicsPipelineCreateInfo);
	vk::Pipeline pipeline = VK_CHECK_RESULT(m_handle.createGraphicsPipeline(nullptr, createInfo), "Failed to create graphics pipeline");
	RHIVulkanPipeline->SetHandle(pipeline);
	//else
	//{
	//	vk::ComputePipelineCreateInfo createInfo = RHIVulkanPipeline->GetComputeCreateInfo(RHISpecs);
	//	vk::Pipeline pipeline = VK_CHECK_RESULT(m_handle.createComputePipeline(nullptr, createInfo), "Failed to create compute pipeline");
	//	RHIVulkanPipeline->SetHandle(pipeline);
	//}
	//
	//RHIVulkanPipeline->SetType(RHISpecs.type);

	return RHIVulkanPipeline;
}

void VulkanDevice::DestroyPipeline(Core::RefCountPtr<Pipeline> RHIPipeline)
{

	Core::RefCountPtr<VulkanGraphicsPipeline> RHIVulkanPipeline = RHIPipeline.CastAs<VulkanGraphicsPipeline>();

	vk::Pipeline pipeline = RHIVulkanPipeline->GetHandle();

	m_handle.destroyPipeline(pipeline);

	vk::PipelineLayout pipelineLayout = RHIVulkanPipeline->GetLayout();
	m_handle.destroyPipelineLayout(pipelineLayout);

	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts = RHIVulkanPipeline->GetDescriptors();
	for (const auto& descriptorSetLayout : descriptorSetLayouts)
		m_handle.destroyDescriptorSetLayout(descriptorSetLayout);
}

// Public Vulkan:

void VulkanDevice::PickPhysicalDevice(const vk::Instance& instance, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, std::vector<const char*> extensions)
{
	m_bSearchPresent = searchPresentQueue;

	std::vector<vk::PhysicalDevice> physicalDevices = VK_CHECK_RESULT(instance.enumeratePhysicalDevices(), "Coudn't enumerate physicalDevice");
	ASSERT(physicalDevices.size() != 0, "failed to find GPUs with Vulkan support!");

	std::vector<PhysicalDevice> candidates;

	for (auto& pd : physicalDevices) 
	{
		PhysicalDevice Device = RatePhysicalDevice(pd, queues, searchPresentQueue, surface, gpuType, extensions);
		candidates.push_back(Device);
	}

	std::sort(candidates.begin(), candidates.end(), 
				[](const auto& a, const auto& b) 
				{
					return a.score > b.score;
				}
			 );

	ASSERT(candidates[0].score > 0, "failed to find a suitable GPU!");

	m_pDevice = candidates[0].physicalDevice;
	
	m_queueFamily = candidates[0].family;

	bool swapchainExtFound = std::find_if(extensions.begin(), extensions.end(), 
											[](const char* ext) 
											{ 
												return std::strcmp(ext, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0; 
											}) != extensions.end();

	if (swapchainExtFound && searchPresentQueue)
	{
		m_compatibility.capabilities = VK_CHECK_RESULT(m_pDevice.getSurfaceCapabilitiesKHR(surface), "Coudn't get surface capabilities");

		m_compatibility.formats = VK_CHECK_RESULT(m_pDevice.getSurfaceFormatsKHR(surface), "Coudn't get surface format");
		ASSERT(m_compatibility.formats.size() != 0, "No surface formats available");

		m_compatibility.presentModes = VK_CHECK_RESULT(m_pDevice.getSurfacePresentModesKHR(surface), "Coudn't get surface present mode");
		ASSERT(m_compatibility.presentModes.size() != 0, "No surface present mode available");
	}
}

void VulkanDevice::CreateLogicalDevice(std::vector<const char*>& extensions)
{
	m_extensions = extensions;

	std::unordered_set<uint32_t> uniqueFamilies;
	for (const auto& [queueType, familyIndexOpt] : m_queueFamily.GetQueues())
	{
		if (familyIndexOpt.has_value())
		{
			uniqueFamilies.insert(familyIndexOpt.value());
		}
	}

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	float queuePriority = 1.0f;
	for (const auto& queueIndex : uniqueFamilies)
	{
		vk::DeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.queueFamilyIndex = queueIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	vk::DeviceCreateInfo createInfo;
	// Queues informations
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	// debug layers informations
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
	createInfo.ppEnabledExtensionNames = m_extensions.data();

	// Enable extensions features
	BuildFeaturesChain();
	createInfo.pNext = &m_featuresChain;

	m_handle = VK_CHECK_RESULT(m_pDevice.createDevice(createInfo), "Coudn't create device");

	// Queue setup
	for (auto& [type, index] : m_queueFamily.GetQueues())
	{
		Queue queue;

		vk::QueueFlagBits vkType = TranslateToVulkan(type);
		vk::Queue vkQueue = m_handle.getQueue(index.value(), 0);

		queue.Create(m_handle, vkQueue, index.value(), vkType);

		m_queues.insert(std::make_pair(type, queue));
	}

	if (m_bSearchPresent)
	{
		uint32_t presentQueueIndex = m_queueFamily.GetPresentQueueIndex();

		m_presentQueue = m_handle.getQueue(presentQueueIndex, 0);
	}
}

void VulkanDevice::CreateMemoryAllocator(const vk::Instance& instance)
{
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.instance = instance;
	allocatorCreateInfo.physicalDevice = m_pDevice;
	allocatorCreateInfo.device = m_handle;

	VK_CHECK_VOID(static_cast<vk::Result>(vmaCreateAllocator(&allocatorCreateInfo, &m_memoryAllocator)), "Failed to create memory allocator");

	for (auto& [type, index] : m_queues)
	{
		m_queues[type].SetAllocator(m_memoryAllocator);
	}
}

void VulkanDevice::Destroy()
{
	WaitIdle();

	vmaDestroyAllocator(m_memoryAllocator);

	for (auto& [type, queue] : m_queues)
	{
		queue.Destroy(m_handle);
	}

	m_handle.destroy();
}

void VulkanDevice::DestroyBuffer(vk::Buffer buffer, VmaAllocation allocation)
{
	vmaDestroyBuffer(m_memoryAllocator, buffer, allocation);
}

vk::Format VulkanDevice::CheckFormatCompatibility(vk::Format requestedFormat, vk::ImageTiling tiling, vk::FormatFeatureFlags requiredFeatures)
{
	vk::FormatProperties props = m_pDevice.getFormatProperties(requestedFormat);

	vk::FormatFeatureFlags availableFeatures = (tiling == vk::ImageTiling::eOptimal) ? props.optimalTilingFeatures : props.linearTilingFeatures;

	if ((availableFeatures & requiredFeatures) == requiredFeatures)
	{
		return requestedFormat;
	}

	return vk::Format::eUndefined;
}

// Private Vulkan
PhysicalDevice VulkanDevice::RatePhysicalDevice(const vk::PhysicalDevice& physicalDevice, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, const std::vector<const char*>& requiredExtensions)
{
	PhysicalDevice device;
	device.physicalDevice = physicalDevice;
	device.family = QueueFamily::FindQueueFamily(physicalDevice, queues, searchPresentQueue, surface);
	device.score = 0;

	if (!device.family.IsComplete()) 
	{
		return device;
	}
	device.score += 250;

	vk::PhysicalDeviceProperties properties = device.physicalDevice.getProperties();
	if (properties.deviceType == gpuType) 
	{
		device.score += 500;
	}

	auto availableExtensions = VK_CHECK_RESULT(device.physicalDevice.enumerateDeviceExtensionProperties(), "Can't enumerate device extension properties");
	std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

	for (const auto& ext : availableExtensions) 
	{
		required.erase(ext.extensionName);
	}

	if (!required.empty()) 
	{
		device.score = 0;
		return device;
	}

	device.score += static_cast<uint32_t>(requiredExtensions.size() * 50);

	return device;
}

void VulkanDevice::BuildFeaturesChain()
{
	m_featuresChain = vk::PhysicalDeviceFeatures2{};
	void* currentPNext = nullptr;

	for (const char* extName : nativeExtensions)
	{
		std::string name(extName);

		if (name == VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME)
		{
			features.timelineSemaphore.timelineSemaphore = VK_TRUE;

			features.timelineSemaphore.pNext = currentPNext;
			currentPNext = &features.timelineSemaphore;
		}
		else if (name == VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
		{
			features.sync2Features.synchronization2 = VK_TRUE;

			features.sync2Features.pNext = currentPNext;
			currentPNext = &features.sync2Features;
		}
	}

	for (const char* extName : m_extensions)
	{
		std::string name(extName);

		if (name == VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
		{
			features.dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

			features.dynamicRenderingFeatures.pNext = currentPNext;
			currentPNext = &features.dynamicRenderingFeatures;
		}
		else if (name == VK_EXT_SHADER_OBJECT_EXTENSION_NAME)
		{
			features.shaderObjectFeatures.shaderObject = VK_TRUE;

			features.shaderObjectFeatures.pNext = currentPNext;
			currentPNext = &features.shaderObjectFeatures;
		}
		else if (name == VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME)
		{
			features.extendedDynamicStateFeatures.extendedDynamicState = VK_TRUE;

			features.extendedDynamicStateFeatures.pNext = currentPNext;
			currentPNext = &features.extendedDynamicStateFeatures;
		}
		else if (name == VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME)
		{
			features.extendedDynamicState2Features.extendedDynamicState2 = VK_TRUE;

			features.extendedDynamicState2Features.pNext = currentPNext;
			currentPNext = &features.extendedDynamicState2Features;
		}
	}

	m_featuresChain.pNext = currentPNext;
}

END_NAMESPACE_RHI