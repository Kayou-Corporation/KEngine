#include "Core/RHI/Private/Vulkan/VulkanDevice.hpp"

#include "Core/RHI/Private/Vulkan/VulkanSurface.hpp"
#include <map>

void VulkanDevice::Create(const DeviceSpecs specs)
{

	(void)specs;
}

void VulkanDevice::Destroy()
{

}

void VulkanDevice::ChoosePhysicalDevice(const vk::Instance& instance, const std::vector<Queue>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, std::vector<const char*> extensions)
{
	std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
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

	if (std::find(extensions.begin(), extensions.end(), VK_KHR_SWAPCHAIN_EXTENSION_NAME) != extensions.end() && searchPresentQueue)
	{
		m_compatibility.capabilities = m_pDevice.getSurfaceCapabilitiesKHR(surface);

		m_compatibility.formats = m_pDevice.getSurfaceFormatsKHR(surface);
		ASSERT(m_compatibility.formats.size() != 0, "No surface formats available");

		m_compatibility.presentModes = m_pDevice.getSurfacePresentModesKHR(surface);
		ASSERT(m_compatibility.presentModes.size() != 0, "No surface present mode available");
	}
}

PhysicalDevice VulkanDevice::RatePhysicalDevice(const vk::PhysicalDevice& physicalDevice, const std::vector<Queue>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, std::vector<const char*> extensions)
{
	PhysicalDevice device;
	device.physicalDevice = physicalDevice;
	device.family = QueueFamily::FindQueueFamily(physicalDevice, queues, searchPresentQueue, surface);

	if (device.family.IsComplete())
	{
		device.score += 250;
	}

	vk::PhysicalDeviceProperties properties = device.physicalDevice.getProperties();

	if (properties.deviceType == gpuType)
	{
		device.score += 500;
	}

	std::vector<vk::ExtensionProperties> availablesExtensions = device.physicalDevice.enumerateDeviceExtensionProperties();
	
	for (uint32_t i = 0; i < availablesExtensions.size(); i++)
	{
		for (uint32_t j = 0; j < extensions.size(); ++j)
		{
			if (strcmp(availablesExtensions[i].extensionName, extensions[j]) == 0)
			{
				device.score += 50;
				m_extensions.push_back(extensions[i]);
			}
		}
	}

	return device;
}