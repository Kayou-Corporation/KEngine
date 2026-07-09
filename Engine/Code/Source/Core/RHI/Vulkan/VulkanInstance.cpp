#include "RHI/Vulkan/VulkanInstance.hpp"

#include <spdlog/spdlog.h>
#include "Window.hpp"
#include "WindowRenderer.hpp"

#include "RHI/Vulkan/VulkanTranslate.hpp"
#include "RHI/Vulkan/VulkanUtils.hpp"
#include "RHI/Vulkan/VulkanSurface.hpp"
#include "RHI/Vulkan/VulkanDevice.hpp"

#include "BuildMode.hpp"

BEGIN_NAMESPACE_CORE

void VulkanInstance::Create(const InstanceSpecs& specs)
{
	vk::ApplicationInfo appInfo{};
	appInfo.setPApplicationName(specs.window->GetName().c_str());
	appInfo.setApplicationVersion(VK_MAKE_VERSION(specs.appVersion.major, specs.appVersion.minor, specs.appVersion.patch));
	appInfo.setPEngineName("KEngine");
	appInfo.setEngineVersion(VK_MAKE_VERSION(specs.engineVersion.major, specs.engineVersion.minor, specs.engineVersion.patch));
	appInfo.setApiVersion(VK_API_VERSION_1_4);

	Core::RefCountPtr<Window::WindowRenderer> wRenderer = specs.window->GetWindowRenderer();
	Core::RefCountPtr<Window::VulkanWindowRenderer> wvkRenderer = wRenderer.CastAs<Window::VulkanWindowRenderer>();
	std::vector<const char*> vkExtensions = wvkRenderer->GetVulkanInstanceExtensions();

#ifdef KDEBUG
	vkExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif 

	std::vector<const char*> vkLayers = TranslateToVulkan(specs.debugLayers);
	m_debugLayers = CheckValidationLayersSupport(vkLayers);

	vk::InstanceCreateInfo createInfo{};
	createInfo.setPApplicationInfo(&appInfo);
	createInfo.setEnabledLayerCount(static_cast<uint32_t>(m_debugLayers.size()));
	createInfo.setPEnabledLayerNames(m_debugLayers);
	createInfo.setEnabledExtensionCount(static_cast<uint32_t>(vkExtensions.size()));
	createInfo.setPEnabledExtensionNames(vkExtensions);

	[[maybe_unused]] vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
#ifdef KDEBUG
	debugCreateInfo.setMessageSeverity
	(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

	debugCreateInfo.setMessageType
	(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

	debugCreateInfo.setPfnUserCallback(DebugCallback);
	createInfo.setPNext(&debugCreateInfo);
#endif

	m_handle = VK_CHECK_RESULT(vk::createInstance(createInfo), "Can't create instance");

	const vk::detail::DynamicLoader dl;
	const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

	m_dispatchLoader.init(m_handle, vkGetInstanceProcAddr);

#ifdef KDEBUG
	m_debugHandler = VK_CHECK_RESULT(m_handle.createDebugUtilsMessengerEXT(debugCreateInfo, nullptr, m_dispatchLoader), "Can't create validation layers");
#endif
}

void VulkanInstance::Destroy()
{
#ifdef KDEBUG
	m_handle.destroyDebugUtilsMessengerEXT(m_debugHandler, nullptr, m_dispatchLoader);
#endif

	m_debugLayers.clear();
	m_handle.destroy();
}

SurfaceHandle VulkanInstance::CreateSurface(const SurfaceSpecs& specs)
{
	Core::RefCountPtr<Window::WindowRenderer> wRenderer = specs.window->GetWindowRenderer();
	Core::RefCountPtr<Window::VulkanWindowRenderer> wvkRenderer = wRenderer.CastAs<Window::VulkanWindowRenderer>();
	VkSurfaceKHR vkSurface = wvkRenderer->CreateVulkanSurface(m_handle);

	auto Surface = Core::CreateRefPtr<VulkanSurface>();
	Surface->SetHandle(vkSurface);

	return Surface;
}

void VulkanInstance::DestroySurface(SurfaceHandle surface)
{
	m_handle.destroySurfaceKHR(surface.CastAs<VulkanSurface>()->GetHandle());
}

std::vector<const char*> VulkanInstance::CheckValidationLayersSupport(const std::vector<const char*>& requestedLayers)
{
	std::vector<vk::LayerProperties> availableLayerProperties = VK_CHECK_RESULT(vk::enumerateInstanceLayerProperties(), "Can't get instance layer properties");

	std::vector<const char*> validLayerProperties;

	for (const char* requestedLayer : requestedLayers)
	{
		bool available = false;
		for (const auto& layer : availableLayerProperties)
		{
			if (strcmp(requestedLayer, layer.layerName) == 0)
			{
				available = true;
				break;
			}
		}
		if (available)
		{
			validLayerProperties.push_back(requestedLayer);
		}
		else
		{
			spdlog::debug("[Vulkan] {}", requestedLayer);
		}
	}

	return validLayerProperties;

}

VkBool32 VulkanInstance::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT messageType,
	const vk::DebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
	static_cast<void>(userData);

	std::string type;

	if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
	{
		type = "GENERAL";
	}
	else if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
	{
		type = "VALIDATION";
	}
	else if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
	{
		type = "PERFORMANCE";
	}

	const std::string msg = callbackData->pMessage;

	if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
	{
		spdlog::debug("[Vulkan] ({}) {}", type, msg);
	}
	else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
	{
		spdlog::info("[Vulkan] ({}) {}", type, msg);
	}
	else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	{
		spdlog::warn("[Vulkan] ({}) {}", type, msg);
	}
	else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
	{
		spdlog::error("[Vulkan] ({}) {}", type, msg);
	}

	return VK_FALSE;
}

DeviceHandle VulkanInstance::CreateDevice(const DeviceSpecs& specs)
{
	vk::SurfaceKHR surface = specs.surface.CastAs<VulkanSurface>()->GetHandle();
	vk::PhysicalDeviceType type = TranslateToVulkan(specs.gpuType);
	std::vector<const char*> extensions = TranslateToVulkan(specs.extensions);

	Core::RefCountPtr<VulkanDevice> device = Core::CreateRefPtr<VulkanDevice>();

	device->PickPhysicalDevice(m_handle, specs.queues, specs.searchPresentQueue, surface, type, extensions);

	device->CreateLogicalDevice(extensions);

	device->CreateMemoryAllocator(m_handle);

	return device;
}

void VulkanInstance::DestroyDevice(DeviceHandle device)
{
	device.CastAs<VulkanDevice>()->Destroy();
}

END_NAMESPACE_CORE