#include "RHI/Vulkan/VulkanSurface.hpp"

BEGIN_NAMESPACE_RHI

void VulkanSurface::SetHandle(vk::SurfaceKHR& surface)
{
	m_handle = surface;
}

void VulkanSurface::SetHandle(VkSurfaceKHR& surface)
{
	m_handle = surface;
}

END_NAMESPACE_RHI