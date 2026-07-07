#pragma once

#include "RHI/API/Surface.hpp"
#include "RHI/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_CORE

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

END_NAMESPACE_CORE