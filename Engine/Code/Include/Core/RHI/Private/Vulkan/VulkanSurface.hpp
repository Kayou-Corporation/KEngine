#pragma once

#include "Core/RHI/Public/Surface.hpp"
#include "Core/RHI/Private/Vulkan/VulkanUtils.hpp"

class VulkanSurface : public Surface
{
public:
	VulkanSurface() = default;
	virtual ~VulkanSurface() override = default;

	void SetHandle(vk::SurfaceKHR& surface);
	void SetHandle(VkSurfaceKHR& surface);

	vk::SurfaceKHR GetHandle() { return m_handle; }
	vk::SurfaceKHR& GetHandleRef() { return m_handle; }

private:
	vk::SurfaceKHR m_handle;
};