#pragma once

#include "RHI/API/Syncronisation.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanSemaphore : public Semaphore
{
public:
	VulkanSemaphore() = default;
	virtual ~VulkanSemaphore() override = default;

public:
	void SetHandle(const vk::Semaphore& semaphore) { m_handle = semaphore; }
	vk::Semaphore GetHandle() { return m_handle; }
	vk::Semaphore GetHandleRef() { return m_handle; }

	vk::SemaphoreCreateInfo GetCreateInfo(const SemaphoreSpecs& specs);

private:
	vk::Semaphore m_handle;
};



class VulkanFence : public Fence
{
public:
	VulkanFence() = default;
	virtual ~VulkanFence() override = default;

public:
	void SetHandle(const vk::Fence& fence) { m_handle = fence; }
	vk::Fence GetHandle() { return m_handle; }
	vk::Fence GetHandleRef() { return m_handle; }

private:
	vk::Fence m_handle;
};

END_NAMESPACE_RHI