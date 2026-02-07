#include "Core/RHI/Private/Vulkan/VulkanSurface.hpp"

void VulkanSurface::SetHandle(vk::SurfaceKHR& surface)
{
	m_handle = surface;
}

void VulkanSurface::SetHandle(VkSurfaceKHR& surface)
{
	m_handle = surface;
}