#pragma once

#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#ifdef VULKAN_ENABLE
#include <vulkan/vulkan.h>
#endif

// -------- Window Renderer ----------

class WindowRenderer : public IResource
{
public:
	~WindowRenderer() = default;
};

#ifdef VULKAN_ENABLE
class VulkanWindowRenderer : public RefCounter<WindowRenderer>
{
public:
	VulkanWindowRenderer() = default;
	~VulkanWindowRenderer() = default;

	KENGINE_API virtual std::vector<const char*> GetVulkanInstanceExtensions() = 0;
	KENGINE_API virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;
};
#endif