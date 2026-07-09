#pragma once

#include <vector>

#include "Utils/Memory.hpp"
#include "Export.hpp"

#include "Window.hpp"

#ifdef VULKAN_ENABLE
#include <vulkan/vulkan.h>
#endif

BEGIN_NAMESPACE_WINDOW

// -------- Window Renderer ----------

class WindowRenderer : virtual public Core::IResource
{
public:
	virtual ~WindowRenderer() = default;
};

#ifdef VULKAN_ENABLE
class VulkanWindowRenderer : virtual public WindowRenderer
{
public:
	virtual ~VulkanWindowRenderer() = default;

	KAPI virtual std::vector<const char*> GetVulkanInstanceExtensions() = 0;
	KAPI virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;

protected:
	Window* m_ownerWindow = nullptr;
};
#endif

END_NAMESPACE_WINDOW