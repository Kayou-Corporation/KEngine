#pragma once

#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#ifdef VULKAN_ENABLE
#include <vulkan/vulkan.h>
#endif

// -------- Window Renderer ----------

class WindowRenderer : virtual public IResource
{
public:
	virtual ~WindowRenderer() = default;
};

#ifdef VULKAN_ENABLE
class VulkanWindowRenderer : virtual public WindowRenderer
{
public:
	virtual ~VulkanWindowRenderer() = default;

	KENGINE_API virtual std::vector<const char*> GetVulkanInstanceExtensions() = 0;
	KENGINE_API virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;
};
#endif