#include "RHI/Vulkan/VulkanDevice.hpp"

#include "RHI/Vulkan/VulkanSurface.hpp"
#include "RHI/Vulkan/VulkanUtils.hpp"
#include "RHI/Vulkan/VulkanTranslate.hpp"
#include "RHI/Vulkan/VulkanSwapchain.hpp"
#include "RHI/Vulkan/VulkanBuffer.hpp"
#include "RHI/Vulkan/VulkanImage.hpp"
#include "RHI/Vulkan/VulkanShader.hpp"
#include "RHI/Vulkan/VulkanPipelineCommon.hpp"
#include "RHI/Vulkan/VulkanGraphicsPipeline.hpp"
#include "RHI/Vulkan/VulkanCommandList.hpp"
#include "RHI/Vulkan/VulkanSyncronisation.hpp"
#include "RHI/Vulkan/VulkanDescriptorSet.hpp"
#include "RHI/Vulkan/VulkanSampler.hpp"

#include <map>
#include <set>
#include <string>
#include <spdlog/spdlog.h>

// TODO : Maybe do more cleanup (Review extension system and constant validation for format usage)

DISABLE_WARNINGS

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_CORE


// PUBLIC : 

VulkanDevice::VulkanDevice()
{
	m_shaderCompiler.Initialize();
	m_queueFamily = QueueFamily();
	m_memoryAllocator = nullptr;
}

//----------- Queue / Command --------------//
CommandListHandle VulkanDevice::GetCommandList(QueueType RHIQueueType)
{
	Core::SharedPtr<VulkanCommandList> RHIVulkanCommandList = Core::MakeShared<VulkanCommandList>();

	if (m_queues.find(RHIQueueType) != m_queues.end())
	{
		TrackedCommandBufferPtr commandBuffer = m_queues[RHIQueueType].GetOrCreateCommandBuffer(m_handle);

		RHIVulkanCommandList->SetHandle(commandBuffer);
		RHIVulkanCommandList->SetOwnerQueueType(RHIQueueType);

		return RHIVulkanCommandList;
	}

	spdlog::error("Queue requested doesn't exist");
	return nullptr;
}

void VulkanDevice::SubmitCommandList(CommandListHandle RHICommandList, const SubmitInfo& RHISubmitInfo)
{
	Core::SharedPtr<VulkanCommandList> RHIVulkanCommandList = StaticCast<VulkanCommandList>(RHICommandList);
	
	TrackedCommandBufferPtr commandBuffer = RHIVulkanCommandList->GetHandle();
	QueueType RHICommandBufferQueue = RHIVulkanCommandList->GetOwnerQueueType();

	std::vector<vk::Semaphore> signalSemaphores;
	for (uint32_t i = 0; i < RHISubmitInfo.signalSemaphores.size(); ++i)
	{
		Core::SharedPtr<VulkanSemaphore> RHIVulkanSemaphore = StaticCast<VulkanSemaphore>(RHISubmitInfo.signalSemaphores[i]);

		vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

		signalSemaphores.push_back(semaphore);
	}
	m_queues[RHICommandBufferQueue].PushSignalSemaphores(signalSemaphores, RHISubmitInfo.signalSemaphoresValues);

	std::vector<vk::Semaphore> waitSemaphores;
	for (uint32_t i = 0; i < RHISubmitInfo.waitSemaphores.size(); ++i)
	{
		Core::SharedPtr<VulkanSemaphore> RHIVulkanSemaphore = StaticCast<VulkanSemaphore>(RHISubmitInfo.waitSemaphores[i]);

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
SemaphoreHandle VulkanDevice::CreateSemaphore(const SemaphoreSpecs& RHISpecs)
{
	Core::SharedPtr<VulkanSemaphore> RHIVulkanSemaphore = Core::MakeShared<VulkanSemaphore>();

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

void VulkanDevice::DestroySemaphore(SemaphoreHandle RHISemaphore)
{
	Core::SharedPtr<VulkanSemaphore> RHIVulkanSemaphore = StaticCast<VulkanSemaphore>(RHISemaphore);

	vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandleRef();

	m_handle.destroySemaphore(semaphore);
}

void VulkanDevice::WaitForSemaphore(SemaphoreHandle RHISemaphore, uint64_t waitValue)
{
	Core::SharedPtr<VulkanSemaphore> RHIVulkanSemaphore = StaticCast<VulkanSemaphore>(RHISemaphore);

	vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

	vk::SemaphoreWaitInfo waitInfo{};
	waitInfo.flags = vk::SemaphoreWaitFlagBits::eAny;
	waitInfo.semaphoreCount = 1;
	waitInfo.pSemaphores = &semaphore;
	waitInfo.pValues = &waitValue;

	VK_CHECK_VOID(m_handle.waitSemaphores(waitInfo, UINT64_MAX), "Can't wait semaphore");
}

FenceHandle VulkanDevice::CreateFence()
{
	Core::SharedPtr<VulkanFence> RHIVulkanFence = Core::MakeShared<VulkanFence>();

	vk::FenceCreateInfo createInfo{};
	createInfo.flags = vk::FenceCreateFlagBits::eSignaled;
	vk::Fence fence = VK_CHECK_RESULT(m_handle.createFence(createInfo), "Coudn't create fence");

	RHIVulkanFence->SetHandle(fence);

	return RHIVulkanFence;
}

void VulkanDevice::DestroyFence(FenceHandle RHIFence)
{
	Core::SharedPtr<VulkanFence> RHIVulkanFence = StaticCast<VulkanFence>(RHIFence);

	vk::Fence fence = RHIVulkanFence->GetHandleRef();

	m_handle.destroyFence(fence);
}

void VulkanDevice::WaitForFence(FenceHandle RHIFence)
{
	Core::SharedPtr<VulkanFence> RHIVulkanFence = StaticCast<VulkanFence>(RHIFence);

	vk::Fence fence = RHIVulkanFence->GetHandle();

	VK_CHECK_VOID(m_handle.waitForFences({ fence }, VK_TRUE, UINT64_MAX), "can't wait for fence");
}

void VulkanDevice::ResetFence(FenceHandle RHIFence)
{
	Core::SharedPtr<VulkanFence> RHIVulkanFence = StaticCast<VulkanFence>(RHIFence);

	vk::Fence fence = RHIVulkanFence->GetHandle();

	VK_CHECK_VOID(m_handle.resetFences({ fence }), "Can't reset for fence");
}

//----------- Swapchain --------------// 
// TODO (Eliott) : Do some cleanup on GetCreateInfo
SwapchainHandle VulkanDevice::CreateSwapchain(const SwapchainSpecs& RHISpecs)
{
	Core::SharedPtr<VulkanSwapchain> RHIVulkanSwapchain = Core::MakeShared<VulkanSwapchain>();

	std::unordered_map<int, int> map;

	vk::SurfaceKHR& surface = StaticCast<VulkanSurface>(RHISpecs.surface)->GetHandleRef();

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

void VulkanDevice::DestroySwapchain(SwapchainHandle RHISwapchain)
{
	Core::SharedPtr<VulkanSwapchain> RHIVulkanSwapchain = StaticCast<VulkanSwapchain>(RHISwapchain);

	m_handle.destroySwapchainKHR(RHIVulkanSwapchain->GetHandle());
}

uint32_t VulkanDevice::AcquirreNextImage(SwapchainHandle RHISwapchain, SemaphoreHandle RHISemaphore)
{
	Core::SharedPtr<VulkanSwapchain> RHIVulkanSwapchain = StaticCast<VulkanSwapchain>(RHISwapchain);
	Core::SharedPtr<VulkanSemaphore> RHIVulkanSemaphore = StaticCast<VulkanSemaphore>(RHISemaphore);

	vk::SwapchainKHR swapchain = RHIVulkanSwapchain->GetHandle();
	vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

	uint32_t imageIndex = VK_CHECK_RESULT(m_handle.acquireNextImageKHR(swapchain, UINT64_MAX, semaphore), "Failed to acquired next image");

	return imageIndex;
}

bool VulkanDevice::Present(const PresentInfo& RHIPresentInfo)
{
	std::vector<vk::Semaphore> waitSemaphores;
	for (uint32_t i = 0; i < RHIPresentInfo.waitSemaphores.size(); ++i)
	{
		Core::SharedPtr<VulkanSemaphore> RHIVulkanSemaphore = StaticCast<VulkanSemaphore>(RHIPresentInfo.waitSemaphores[i]);

		vk::Semaphore semaphore = RHIVulkanSemaphore->GetHandle();

		waitSemaphores.push_back(semaphore);
	}

	Core::SharedPtr<VulkanSwapchain> RHIVulkanSwapchain = StaticCast<VulkanSwapchain>(RHIPresentInfo.swapchain);
	vk::SwapchainKHR swapchain = RHIVulkanSwapchain->GetHandle();

	vk::PresentInfoKHR presentInfo;
	presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
	presentInfo.pWaitSemaphores = waitSemaphores.data();
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &RHIPresentInfo.imageIndex;

	vk::Result result = m_presentQueue.presentKHR(presentInfo);
	
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
	{
		return false;
	}

	return true;
}

//-------------- Buffer --------------// 
BufferHandle VulkanDevice::CreateBuffer(const BufferSpecs& RHISpecs)
{
	Core::SharedPtr<VulkanBuffer> RHIVulkanBuffer = Core::MakeShared<VulkanBuffer>();

	VkBufferCreateInfo bufferInfo = RHIVulkanBuffer->GetCreateInfo(RHISpecs);

	VmaAllocationCreateInfo allocInfo = TranslateToVulkan(RHISpecs.memoryAccess);
	if (RHIVulkanBuffer->GetIsPersistentMapped() && RHISpecs.memoryAccess != MemoryAccess::GpuOnly)
		allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VkBuffer buf;
	VmaAllocation allocation;
	VmaAllocationInfo allocationInfo;
	VK_CHECK_VOID(static_cast<vk::Result>(vmaCreateBuffer(m_memoryAllocator, &bufferInfo, &allocInfo, &buf, &allocation, &allocationInfo)), "Failed to create buffer");

	RHIVulkanBuffer->SetHandle(static_cast<vk::Buffer>(buf));
	RHIVulkanBuffer->SetAllocation(allocation);
	RHIVulkanBuffer->SetAllocationInfo(allocationInfo);

	if (RHISpecs.memoryAccess == MemoryAccess::GpuOnly || RHISpecs.memoryAccess == MemoryAccess::Dynamic)
		RHIVulkanBuffer->SetIsGpuOnly(true);

	return RHIVulkanBuffer;
}

void VulkanDevice::DestroyBuffer(BufferHandle RHIBuffer)
{
	Core::SharedPtr<VulkanBuffer> RHIVulkanBuffer = StaticCast<VulkanBuffer>(RHIBuffer);

	VkBuffer rawBuffer = static_cast<VkBuffer>(RHIVulkanBuffer->GetHandle());
	VmaAllocation bufferAllocation = RHIVulkanBuffer->GetAllocation();

	vmaDestroyBuffer(m_memoryAllocator, rawBuffer, bufferAllocation);
}

//-------------- Image --------------// 
ImageHandle VulkanDevice::CreateImage(const ImageSpecs& RHISpecs)
{
	Core::SharedPtr<VulkanImage> RHIVulkanImage = Core::MakeShared<VulkanImage>();

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

void VulkanDevice::DestroyImage(ImageHandle RHIImage)
{
	Core::SharedPtr<VulkanImage> RHIVulkanImage = StaticCast<VulkanImage>(RHIImage);

	vk::Image image = RHIVulkanImage->GetHandleRef();
	vk::ImageView imageView = RHIVulkanImage->GetHandleViewRef();
	VmaAllocation imageAllocation = RHIVulkanImage->GetAllocation();

	m_handle.destroyImageView(imageView);

	vmaDestroyImage(m_memoryAllocator, image, imageAllocation);
}

std::vector<ImageHandle> VulkanDevice::CreatePresentationImages(SwapchainHandle RHISwapchain)
{
	Core::SharedPtr<VulkanSwapchain> RHIVulkanSwapchain = StaticCast<VulkanSwapchain>(RHISwapchain);
	uint32_t swapchainImageCount = RHIVulkanSwapchain->GetImageCount();

	std::vector<ImageHandle> RHIImages(swapchainImageCount);

	vk::SwapchainKHR swapchain = RHIVulkanSwapchain->GetHandle();
	std::vector<vk::Image> swapchainImages = VK_CHECK_RESULT(m_handle.getSwapchainImagesKHR(swapchain), "Coudn't extract images from swapchain");

	ASSERT(swapchainImageCount == RHIImages.size(), "Swapchain images count don't match acquired images");

	vk::Format colorImageFormat = RHIVulkanSwapchain->GetColorImageFormat();
	vk::Extent2D imageExtent = RHIVulkanSwapchain->GetImageExtent();

	for (uint32_t i = 0; i < RHIImages.size(); ++i)
	{
		Core::SharedPtr<VulkanImage> RHIVulkanImage = Core::MakeShared<VulkanImage>();

		RHIVulkanImage->SetHandle(swapchainImages[i]);

		vk::ImageViewCreateInfo imageViewCreateInfo = RHIVulkanImage->GetViewCreateInfoForPresentation(colorImageFormat, { imageExtent.width, imageExtent.height, 0 });

		vk::ImageView imageView = VK_CHECK_RESULT(m_handle.createImageView(imageViewCreateInfo, nullptr), "Failed to create image");


		RHIVulkanImage->SetHandleView(imageView);

		RHIImages[i] = RHIVulkanImage;
	}

	return RHIImages;
}

void VulkanDevice::DestroyPresentationImages(std::vector<ImageHandle> RHIPresentationImages)
{
	// With swapchain images you only destroy their vk::ImageView and not the vk::Image
	for (uint32_t i = 0; i < RHIPresentationImages.size(); ++i)
	{
		Core::SharedPtr<VulkanImage> RHIVulkanImage = StaticCast<VulkanImage>(RHIPresentationImages[i]);
		vk::ImageView imageViewHandle = RHIVulkanImage->GetHandleViewRef();

		m_handle.destroyImageView(imageViewHandle);
	}
}

ImageHandle VulkanDevice::CreateImagesWithSwapchain(const SwapchainImageSpecs& RHISpecs, SwapchainHandle RHISwapchain)
{
	Core::SharedPtr<VulkanSwapchain> RHIVulkanSwapchain = StaticCast<VulkanSwapchain>(RHISwapchain);
	[[maybe_unused]] uint32_t swapchainImageCount = RHIVulkanSwapchain->GetImageCount();

	Core::SharedPtr<VulkanImage> RHIVulkanImage = Core::MakeShared<VulkanImage>();

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

// -------------- Sampler -------------- // 
SamplerHandle VulkanDevice::CreateSampler(const SamplerSpecs& specs)
{
	Core::SharedPtr<VulkanSampler> RHIVulkanSampler = Core::MakeShared<VulkanSampler>();

	vk::PhysicalDeviceProperties properties = m_pDevice.getProperties();
	float maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	vk::SamplerCreateInfo createInfo = RHIVulkanSampler->GetCreateInfo(specs, maxAnisotropy);
	vk::Sampler sampler = VK_CHECK_RESULT(m_handle.createSampler(createInfo), "Coudn't create sampler");

	RHIVulkanSampler->SetHandle(sampler);

	return RHIVulkanSampler;
}

void VulkanDevice::DestroySampler(SamplerHandle RHISampler)
{
	Core::SharedPtr<VulkanSampler> RHIVulkanSampler = StaticCast<VulkanSampler>(RHISampler);

	vk::Sampler sampler = RHIVulkanSampler->GetHandle();
	m_handle.destroySampler(sampler);
}

//-------------- Shader --------------// 
ShaderHandle VulkanDevice::CreateShader(const std::string& file, const ShaderStage& sStage, bool isGlobalLayout, bool usesGlobalLayout)
{
	Core::SharedPtr<VulkanShader> shader{};

	if ((shader = StaticCast<VulkanShader>(m_compiledShaders[file])))
		return shader;

	shader = Core::MakeShared<VulkanShader>();

	ShaderData bin = m_shaderCompiler.Load(file, sStage, isGlobalLayout, usesGlobalLayout, RendererAPI::Vulkan);

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
	shader->SetPushConstants(bin.pushConstants);
	shader->SetVertexAttributes(bin.vertexAttributes);
	shader->SetVertexBindings(bin.vertexBindings);
	shader->SetIsGlobalLayout(isGlobalLayout);
	shader->SetUsesGlobalLayout(usesGlobalLayout);

	m_compiledShaders[file] = shader;

	return shader;
}

void VulkanDevice::DestroyShader(ShaderHandle shader)
{
	vk::ShaderModule shaderModule = StaticCast<VulkanShader>(shader)->GetModule();

	m_handle.destroyShaderModule(shaderModule);
}

// -------------- DescriptorSetLayout -------------- // 
std::vector<DescriptorSetLayoutHandle> VulkanDevice::CreateDescriptorSetsLayouts(ShaderHandle RHIShader)
{
	std::vector<DescriptorSetLayoutHandle> RHIDescriptors;

	std::vector<Descriptor> SLANGShaderDescriptors = RHIShader->GetDescriptors();
	for (const Descriptor& SLANGDescriptor : SLANGShaderDescriptors)
	{
		Core::SharedPtr<VulkanDescriptorSetLayout> RHIVulkanDescriptor = Core::MakeShared<VulkanDescriptorSetLayout>();
		RHIVulkanDescriptor->SetName(SLANGDescriptor.name);
		RHIVulkanDescriptor->SetIndex(SLANGDescriptor.index);

		for (const Binding& SLANGBinding : SLANGDescriptor.bindings)
		{
			VulkanBinding binding;
			binding.name = SLANGBinding.name;

			vk::DescriptorSetLayoutBinding vulkanBinding;
			vulkanBinding.binding = SLANGBinding.index;
			vulkanBinding.descriptorType = TranslateToVulkan(SLANGBinding.type);
			vulkanBinding.stageFlags = TranslateToVulkan(SLANGBinding.stage);
			vulkanBinding.descriptorCount = SLANGBinding.count;
			binding.binding = vulkanBinding;

			RHIVulkanDescriptor->AddBinding(binding);
		}

		std::vector<vk::DescriptorSetLayoutBinding>& allBindings = RHIVulkanDescriptor->GetAllVulkanBindings();

		vk::DescriptorSetLayoutCreateInfo createInfo{};
		createInfo.flags = {};
		createInfo.bindingCount = static_cast<uint32_t>(allBindings.size());
		createInfo.pBindings = allBindings.data();

		vk::DescriptorSetLayout layout = VK_CHECK_RESULT(m_handle.createDescriptorSetLayout(createInfo), "Coundn't create DescriptorSetLayout");
		RHIVulkanDescriptor->SetHandle(layout);

		RHIDescriptors.push_back(RHIVulkanDescriptor);
	}

	return RHIDescriptors;
}

DescriptorSetLayoutHandle VulkanDevice::CreateDescriptorSetLayout(ShaderHandle RHIShader, std::string name)
{
	Core::SharedPtr<VulkanDescriptorSetLayout> RHIVulkanDescriptor = Core::MakeShared<VulkanDescriptorSetLayout>();

	std::vector<Descriptor> SLANGShaderDescriptors = RHIShader->GetDescriptors();
	for (const Descriptor& SLANGDescriptor : SLANGShaderDescriptors)
	{
		if (SLANGDescriptor.name == name)
		{
			RHIVulkanDescriptor->SetName(SLANGDescriptor.name);
			RHIVulkanDescriptor->SetIndex(SLANGDescriptor.index);
			for (const Binding& SLANGBinding : SLANGDescriptor.bindings)
			{
				VulkanBinding binding;
				binding.name = SLANGBinding.name;

				vk::DescriptorSetLayoutBinding vulkanBinding;
				vulkanBinding.binding = SLANGBinding.index;
				vulkanBinding.descriptorType = TranslateToVulkan(SLANGBinding.type);
				vulkanBinding.stageFlags = TranslateToVulkan(SLANGBinding.stage);
				vulkanBinding.descriptorCount = SLANGBinding.count;
				binding.binding = vulkanBinding;

				RHIVulkanDescriptor->AddBinding(binding);
			}

			std::vector<vk::DescriptorSetLayoutBinding> allBindings = RHIVulkanDescriptor->GetAllVulkanBindings();

			vk::DescriptorSetLayoutCreateInfo createInfo{};
			createInfo.bindingCount = static_cast<uint32_t>(allBindings.size());
			createInfo.pBindings = allBindings.data();

			vk::DescriptorSetLayout layout = VK_CHECK_RESULT(m_handle.createDescriptorSetLayout(createInfo), "Coundn't create DescriptorSetLayout");
			RHIVulkanDescriptor->SetHandle(layout);

			break;
		}
	}

	return RHIVulkanDescriptor;
}

DescriptorSetLayoutHandle VulkanDevice::CreateDescriptorSetLayout(ShaderHandle RHIShader, uint32_t index)
{
	Core::SharedPtr<VulkanDescriptorSetLayout> RHIVulkanDescriptor = Core::MakeShared<VulkanDescriptorSetLayout>();

	std::vector<Descriptor> SLANGShaderDescriptors = RHIShader->GetDescriptors();
	for (const Descriptor& SLANGDescriptor : SLANGShaderDescriptors)
	{
		if (SLANGDescriptor.index == index)
		{
			RHIVulkanDescriptor->SetName(SLANGDescriptor.name);
			RHIVulkanDescriptor->SetIndex(SLANGDescriptor.index);
			for (const Binding& SLANGBinding : SLANGDescriptor.bindings)
			{
				VulkanBinding binding;
				binding.name = SLANGBinding.name;

				vk::DescriptorSetLayoutBinding vulkanBinding;
				vulkanBinding.binding = SLANGBinding.index;
				vulkanBinding.descriptorType = TranslateToVulkan(SLANGBinding.type);
				vulkanBinding.stageFlags = TranslateToVulkan(SLANGBinding.stage);
				vulkanBinding.descriptorCount = SLANGBinding.count;
				binding.binding = vulkanBinding;

				RHIVulkanDescriptor->AddBinding(binding);
			}

			std::vector<vk::DescriptorSetLayoutBinding> allBindings = RHIVulkanDescriptor->GetAllVulkanBindings();

			vk::DescriptorSetLayoutCreateInfo createInfo{};
			createInfo.bindingCount = static_cast<uint32_t>(allBindings.size());
			createInfo.pBindings = allBindings.data();

			vk::DescriptorSetLayout layout = VK_CHECK_RESULT(m_handle.createDescriptorSetLayout(createInfo), "Coundn't create DescriptorSetLayout");
			RHIVulkanDescriptor->SetHandle(layout);

			break;
		}
	}

	return RHIVulkanDescriptor;
}

void VulkanDevice::DestroyDescriptorSetsLayouts(std::vector<DescriptorSetLayoutHandle> RHIDescriptors)
{
	for (const DescriptorSetLayoutHandle& RHIDescriptorSetLayout : RHIDescriptors)
	{
		Core::SharedPtr<VulkanDescriptorSetLayout> RHIVulkanDescriptorSetLayout = StaticCast<VulkanDescriptorSetLayout>(RHIDescriptorSetLayout);
		
		m_handle.destroyDescriptorSetLayout(RHIVulkanDescriptorSetLayout->GetHandle());
	}
}

void VulkanDevice::DestroyDescriptorSetLayout(DescriptorSetLayoutHandle RHIDescriptor)
{
	Core::SharedPtr<VulkanDescriptorSetLayout> RHIVulkanDescriptorSetLayout = StaticCast<VulkanDescriptorSetLayout>(RHIDescriptor);

	m_handle.destroyDescriptorSetLayout(RHIVulkanDescriptorSetLayout->GetHandle());
}

// -------------- PushConstantLayout -------------- // 
PushConstantLayoutHandle VulkanDevice::CreatePushConstantLayout(ShaderHandle RHIShader)
{
	Core::SharedPtr<VulkanPushConstantLayout> RHIVulkanPushConstants = Core::MakeShared<VulkanPushConstantLayout>();

	std::vector<PushConstant> SLANGShaderPushConstants = RHIShader->GetPushConstants();
	if (!SLANGShaderPushConstants.empty())
	{
		const PushConstant globalStruct = SLANGShaderPushConstants[0];
		RHIVulkanPushConstants->SetName(globalStruct.name);
		RHIVulkanPushConstants->SetSize(static_cast<uint32_t>(globalStruct.size));
		RHIVulkanPushConstants->SetStage(globalStruct.stage);

		for (int i = static_cast<int>(SLANGShaderPushConstants.size()) - 1; i > 0; i--)
		{
			VulkanConstant constant;
			constant.constantName = SLANGShaderPushConstants[i].name;
			constant.offset = SLANGShaderPushConstants[i].offset;
			constant.size = static_cast<uint32_t>(SLANGShaderPushConstants[i].size);

			RHIVulkanPushConstants->AddConstant(constant);
		}

		vk::PushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = TranslateToVulkan(globalStruct.stage);
		pushConstantRange.size = static_cast<uint32_t>(globalStruct.size);
		pushConstantRange.offset = globalStruct.offset;

		RHIVulkanPushConstants->SetHandle(pushConstantRange);
	}

	return RHIVulkanPushConstants;
}

void VulkanDevice::DestroyPushConstantsLayouts(std::vector<PushConstantLayoutHandle>& pushConstants)
{
	// Empty
	pushConstants.clear();
}

// -------------- Descriptor Set -------------- // 
DescriptorSetHandle VulkanDevice::CreateDescriptorSet(DescriptorSetLayoutHandle RHILayout)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = Core::MakeShared<VulkanDescriptorSet>();

	Core::SharedPtr<VulkanDescriptorSetLayout> RHIVulkanLayout = StaticCast<VulkanDescriptorSetLayout>(RHILayout);

	std::vector<VulkanBinding> bindings = RHIVulkanLayout->GetAllBindings();

	std::map<vk::DescriptorType, uint32_t> descriptors;
	uint32_t totalDescriptorsCount = 0;
	for (const VulkanBinding& binding : bindings)
	{
		if (descriptors.find(binding.binding.descriptorType) != descriptors.end())
		{
			descriptors[binding.binding.descriptorType] += binding.binding.descriptorCount;
		}
		else
		{
			descriptors.emplace(binding.binding.descriptorType, binding.binding.descriptorCount);
		}
		totalDescriptorsCount += binding.binding.descriptorCount;
	}
	RHIVulkanDescriptorSet->SetBindings(bindings);

	vk::DescriptorPoolCreateInfo poolCreateInfo = RHIVulkanDescriptorSet->GetPoolCreateInfo(descriptors, totalDescriptorsCount);
	vk::DescriptorPool pool = VK_CHECK_RESULT(m_handle.createDescriptorPool(poolCreateInfo), "Coundn't create Descriptor pool");
	RHIVulkanDescriptorSet->SetPool(pool);

	vk::DescriptorSetLayout layout = RHIVulkanLayout->GetHandle();
	vk::DescriptorSetAllocateInfo allocInfo = RHIVulkanDescriptorSet->GetAllocInfo(layout);
	allocInfo.pSetLayouts = &layout;

	std::vector<vk::DescriptorSet> descriptorSet = VK_CHECK_RESULT(m_handle.allocateDescriptorSets(allocInfo), "Coundn't allocate descriptor sets");
	
	RHIVulkanDescriptorSet->SetHanlde(descriptorSet[0]);

	return RHIVulkanDescriptorSet;
}

void VulkanDevice::DestroyDescriptorSet(DescriptorSetHandle RHIDescriptorSet)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = StaticCast<VulkanDescriptorSet>(RHIDescriptorSet);

	vk::DescriptorPool pool = RHIVulkanDescriptorSet->GetPool();

	m_handle.destroyDescriptorPool(pool);
}

void VulkanDevice::SetDescriptorSetBuffer(DescriptorSetHandle RHIDescriptorSet, uint32_t index, DescriptorType type, BufferHandle RHIBuffer, uint32_t offset, uint32_t range)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = StaticCast<VulkanDescriptorSet>(RHIDescriptorSet);

	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::Buffer updateBuffer = StaticCast<VulkanBuffer>(RHIBuffer)->GetHandle();
	std::vector<VulkanBinding> vulkanBindings = RHIVulkanDescriptorSet->GetAllBindings();

	for (const VulkanBinding& vulkanBinding : vulkanBindings)
	{
		if (vulkanBinding.binding.binding == index && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = updateBuffer;
			bufferInfo.offset = offset;
			bufferInfo.range = range;

			vk::WriteDescriptorSet write{};
			write.dstSet = descriptorSet;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfo;

			m_handle.updateDescriptorSets(write, nullptr);

			break;
		}
	}
}

void VulkanDevice::SetDescriptorSetBuffer(DescriptorSetHandle RHIDescriptorSet, std::string name, DescriptorType type, BufferHandle RHIBuffer, uint32_t offset, uint32_t range)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = StaticCast<VulkanDescriptorSet>(RHIDescriptorSet);

	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::Buffer updateBuffer = StaticCast<VulkanBuffer>(RHIBuffer)->GetHandle();
	std::vector<VulkanBinding> vulkanBindings = RHIVulkanDescriptorSet->GetAllBindings();

	for (const VulkanBinding& vulkanBinding : vulkanBindings)
	{
		if (vulkanBinding.name == name && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = updateBuffer;
			bufferInfo.offset = offset;
			bufferInfo.range = range;

			vk::WriteDescriptorSet write{};
			write.dstSet = descriptorSet;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfo;

			m_handle.updateDescriptorSets(write, nullptr);

			break;
		}
	}
}

void VulkanDevice::SetDescriptorSetImage(DescriptorSetHandle RHIDescriptorSet, std::string name, DescriptorType type, ImageHandle RHIImage, SamplerHandle RHISampler)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = StaticCast<VulkanDescriptorSet>(RHIDescriptorSet);

	Core::SharedPtr<VulkanImage> RHIVulkanImage = StaticCast<VulkanImage>(RHIImage);

	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::ImageView updateImageView = RHIVulkanImage->GetHandleView();

	vk::Sampler updateSampler;
	if (RHISampler)
	{
		updateSampler = StaticCast<VulkanSampler>(RHISampler)->GetHandle();
	}
	std::vector<VulkanBinding> vulkanBindings = RHIVulkanDescriptorSet->GetAllBindings();

	vk::DescriptorType vkType = TranslateToVulkan(type);

	for (const VulkanBinding& vulkanBinding : vulkanBindings)
	{
		if (vulkanBinding.name == name && vulkanBinding.binding.descriptorType == vkType)
		{
			vk::DescriptorImageInfo imageInfo{};
			imageInfo.imageView = updateImageView;
			imageInfo.imageLayout = RHIVulkanImage->GetVulkanCurrentLayout();
			if (vkType == vk::DescriptorType::eCombinedImageSampler)
			{
				imageInfo.sampler = updateSampler;
			}

			vk::WriteDescriptorSet write{};
			write.dstSet = descriptorSet;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;

			m_handle.updateDescriptorSets(write, nullptr);

			break;
		}
	}
}

void VulkanDevice::SetDescriptorSetImage(DescriptorSetHandle RHIDescriptorSet, uint32_t index, DescriptorType type, ImageHandle RHIImage, SamplerHandle RHISampler)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = StaticCast<VulkanDescriptorSet>(RHIDescriptorSet);

	Core::SharedPtr<VulkanImage> RHIVulkanImage = StaticCast<VulkanImage>(RHIImage);

	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::ImageView updateImageView = RHIVulkanImage->GetHandleView();
	vk::Sampler updateSampler;
	if (RHISampler)
	{
		updateSampler = StaticCast<VulkanSampler>(RHISampler)->GetHandle();
	}
	std::vector<VulkanBinding> vulkanBindings = RHIVulkanDescriptorSet->GetAllBindings();

	vk::DescriptorType vkType = TranslateToVulkan(type);

	for (const VulkanBinding& vulkanBinding : vulkanBindings)
	{
		if (vulkanBinding.binding.binding == index && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			vk::DescriptorImageInfo imageInfo{};
			imageInfo.imageView = updateImageView;
			imageInfo.imageLayout = RHIVulkanImage->GetVulkanCurrentLayout();
			if (vkType == vk::DescriptorType::eCombinedImageSampler)
			{
				imageInfo.sampler = updateSampler;
			}

			vk::WriteDescriptorSet write{};
			write.dstSet = descriptorSet;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;

			m_handle.updateDescriptorSets(write, nullptr);

			break;
		}
	}
}

void VulkanDevice::SetDescriptorSetSampler(DescriptorSetHandle RHIDescriptorSet, std::string name, DescriptorType type, SamplerHandle RHISampler)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = StaticCast<VulkanDescriptorSet>(RHIDescriptorSet);

	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::Sampler updateSampler = StaticCast<VulkanSampler>(RHISampler)->GetHandle();
	std::vector<VulkanBinding> vulkanBindings = RHIVulkanDescriptorSet->GetAllBindings();

	for (const VulkanBinding& vulkanBinding : vulkanBindings)
	{
		if (vulkanBinding.name == name && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			vk::DescriptorImageInfo imageInfo{};
			imageInfo.sampler = updateSampler;

			vk::WriteDescriptorSet write{};
			write.dstSet = descriptorSet;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;

			m_handle.updateDescriptorSets(write, nullptr);

			break;
		}
	}
}

void VulkanDevice::SetDescriptorSetSampler(DescriptorSetHandle RHIDescriptorSet, uint32_t index, DescriptorType type, SamplerHandle RHISampler)
{
	Core::SharedPtr<VulkanDescriptorSet> RHIVulkanDescriptorSet = StaticCast<VulkanDescriptorSet>(RHIDescriptorSet);

	vk::DescriptorSet descriptorSet = RHIVulkanDescriptorSet->GetHandle();
	vk::Sampler updateSampler = StaticCast<VulkanSampler>(RHISampler)->GetHandle();
	std::vector<VulkanBinding> vulkanBindings = RHIVulkanDescriptorSet->GetAllBindings();

	for (const VulkanBinding& vulkanBinding : vulkanBindings)
	{
		if (vulkanBinding.binding.binding == index && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			vk::DescriptorImageInfo imageInfo{};
			imageInfo.sampler = updateSampler;

			vk::WriteDescriptorSet write{};
			write.dstSet = descriptorSet;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;

			m_handle.updateDescriptorSets(write, { descriptorSet });

			break;
		}
	}
}

// -------------- Pipeline Layout -------------- // 
PipelineLayoutHandle VulkanDevice::CreatePipelineLayout(std::vector<DescriptorSetLayoutHandle> descriptors, std::vector<PushConstantLayoutHandle> pushConstants)
{
	Core::SharedPtr<VulkanPipelineLayout> RHIVUlkanPipelineLayout = Core::MakeShared<VulkanPipelineLayout>();

	std::vector<vk::DescriptorSetLayout> descriptorSetsLayouts;
	for (const DescriptorSetLayoutHandle& RHIDescriptor : descriptors)
	{
		Core::SharedPtr<VulkanDescriptorSetLayout> RHIVulkanDescriptor = StaticCast<VulkanDescriptorSetLayout>(RHIDescriptor);

		descriptorSetsLayouts.push_back(RHIVulkanDescriptor->GetHandle());
		RHIVUlkanPipelineLayout->AddDescriptorSetLayout(RHIDescriptor);
	}

	std::vector<vk::PushConstantRange> pushConstantRanges;
	for (const PushConstantLayoutHandle& RHIPushConstant: pushConstants)
	{
		Core::SharedPtr<VulkanPushConstantLayout> RHIVulkanPushConstant = StaticCast<VulkanPushConstantLayout>(RHIPushConstant);

		pushConstantRanges.push_back(RHIVulkanPushConstant->GetHandle());
		RHIVUlkanPipelineLayout->AddPushConstantLayout(RHIPushConstant);
	}

	vk::PipelineLayoutCreateInfo createInfo{};
	createInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetsLayouts.size());
	createInfo.pSetLayouts = descriptorSetsLayouts.data();
	createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	createInfo.pPushConstantRanges = pushConstantRanges.data();

	vk::PipelineLayout pipelineLayout = VK_CHECK_RESULT(m_handle.createPipelineLayout(createInfo), "Coudn't create pipeline layout");
	RHIVUlkanPipelineLayout->SetHandle(pipelineLayout);

	return RHIVUlkanPipelineLayout;
}

void VulkanDevice::DestroyPipelineLayout(PipelineLayoutHandle RHIPipelineLayout)
{
	Core::SharedPtr<VulkanPipelineLayout> RHIVUlkanPipelineLayout = StaticCast<VulkanPipelineLayout>(RHIPipelineLayout);

	vk::PipelineLayout layout = RHIVUlkanPipelineLayout->GetHandle();
	m_handle.destroyPipelineLayout(layout);
}

//-------------- Pipeline --------------// 
GraphicsPipelineHandle VulkanDevice::CreateGraphicsPipeline(const GraphicsPipelineSpecs& RHISpecs)
{
	Core::SharedPtr<VulkanGraphicsPipeline> RHIVulkanPipeline = Core::MakeShared<VulkanGraphicsPipeline>();
	RHIVulkanPipeline->SetLayout(RHISpecs.pipelineLayout);
	RHIVulkanPipeline->SetType(PipelineType::Graphics);

	VulkanGraphicsPipelineStructs RHIVulkanGraphicsPipelineCreateInfo = RHIVulkanPipeline->GetGraphicsCreateInfo(RHISpecs);
	vk::GraphicsPipelineCreateInfo createInfo = RHIVulkanPipeline->GetVulkanGraphicsCreateInfo(RHIVulkanGraphicsPipelineCreateInfo);
	vk::Pipeline pipeline = VK_CHECK_RESULT(m_handle.createGraphicsPipeline(nullptr, createInfo), "Failed to create graphics pipeline");
	RHIVulkanPipeline->SetHandle(pipeline);

	return RHIVulkanPipeline;
}

void VulkanDevice::DestroyPipeline(PipelineHandle RHIPipeline)
{
	if (RHIPipeline->GetType() == PipelineType::Graphics)
	{
		Core::SharedPtr<VulkanGraphicsPipeline> RHIVulkanPipeline = StaticCast<VulkanGraphicsPipeline>(RHIPipeline);
		vk::Pipeline pipeline = RHIVulkanPipeline->GetHandle();
		m_handle.destroyPipeline(pipeline);
	}
	else
	{

	}
}

void VulkanDevice::UpdateCompatibility(SurfaceHandle RHISurface)
{
	Core::SharedPtr<VulkanSurface> RHIVulkanSurface = StaticCast<VulkanSurface>(RHISurface);

	vk::SurfaceKHR surface = RHIVulkanSurface->GetHandle();

	m_compatibility.capabilities = VK_CHECK_RESULT(m_pDevice.getSurfaceCapabilitiesKHR(surface), "Coudn't get surface capabilities");

	m_compatibility.formats = VK_CHECK_RESULT(m_pDevice.getSurfaceFormatsKHR(surface), "Coudn't get surface format");
	ASSERT(m_compatibility.formats.size() != 0, "No surface formats available");

	m_compatibility.presentModes = VK_CHECK_RESULT(m_pDevice.getSurfacePresentModesKHR(surface), "Coudn't get surface present mode");
	ASSERT(m_compatibility.presentModes.size() != 0, "No surface present mode available");
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

		vk::PhysicalDeviceProperties properties = pd.getProperties();
		std::string gpuName = properties.deviceName;
		spdlog::info(gpuName);
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

END_NAMESPACE_CORE