#include "Core/RHI/Private/Vulkan/VulkanDevice.hpp"

#include "Core/RHI/Private/Vulkan/VulkanSurface.hpp"
#include "Core/RHI/Private/Vulkan/VulkanUtils.hpp"
#include "Core/RHI/Private/Vulkan/VulkanQueue.hpp"
#include "Core/RHI/Private/Vulkan/VulkanTranslate.hpp"
#include "Core/RHI/Private/Vulkan/VulkanSwapchain.hpp"

#include <map>
#include <set>
#include <string>

BEGIN_NAMESPACE_CORE

void VulkanDevice::WaitIdle()
{
	VK_CHECK_VOID(m_handle.waitIdle(), "Device can't wait idle");
}

void VulkanDevice::QueueWaitIdle(QueueType type)
{
	m_queues[type].WaitIdle();
}

RefCountPtr<Swapchain> VulkanDevice::CreateSwapchain(const SwapchainSpecs& specs)
{
	RefCountPtr<VulkanSwapchain> swapchain = CreateRefPtr<VulkanSwapchain>();

	std::unordered_map<int, int> map;

	vk::SurfaceKHR& surface = specs.surface.CastAs<VulkanSurface>()->GetHandleRef();

	uint32_t graphicsQueueIndex = m_queueFamily.GetQueues().at(QueueType::Graphics).value();
	uint32_t presentQueueIndex = m_queueFamily.GetPresentQueueIndex();

	vk::Format requestedFormat = TranslateToVulkan(specs.imageFormat);
	vk::Format requestedDepthFormat = TranslateToVulkan(specs.depthImageFormat);

	vk::PresentModeKHR requestedpresentMode = TranslateToVulkan(specs.presentMode);

	vk::Extent2D requestedExtent = TranslateToVulkan(specs.extent);

	vk::SwapchainCreateInfoKHR createInfo = swapchain->GetCreateInfo(m_compatibility, surface, graphicsQueueIndex, presentQueueIndex, specs.imageCount, requestedFormat, specs.isDepthEnable, requestedDepthFormat, requestedpresentMode, requestedExtent);

	vk::SwapchainKHR vkSwapchain = VK_CHECK_RESULT(m_handle.createSwapchainKHR(createInfo), "Can't create swapchain");

	swapchain->SetHandle(vkSwapchain);

	return swapchain;
}

void VulkanDevice::DestroySwapchain(RefCountPtr<Swapchain> swapchain)
{
	auto vkSwapchain = swapchain.CastAs<VulkanSwapchain>();
	
	m_handle.destroySwapchainKHR(vkSwapchain->GetHandle());
}

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

PhysicalDevice VulkanDevice::RatePhysicalDevice(const vk::PhysicalDevice& physicalDevice, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, const std::vector<const char*>& requiredExtensions)
{
	PhysicalDevice device;
	device.physicalDevice = physicalDevice;
	device.family = QueueFamily::FindQueueFamily(physicalDevice, queues, searchPresentQueue, surface);

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
		uint32_t presentQueueIndex =  m_queueFamily.GetPresentQueueIndex();

		m_presentQueue = m_handle.getQueue(presentQueueIndex, 0);
	}
}

void VulkanDevice::Destroy()
{
	WaitIdle();

	for (auto& [type, queue] : m_queues)
	{
		queue.Destroy(m_handle);
	}

	m_handle.destroy();
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