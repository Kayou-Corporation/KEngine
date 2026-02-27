#include "Core/RHI/Private/Vulkan/VulkanCommandList.hpp"

void VulkanCommandList::Open()
{
	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	VK_CHECK_VOID(m_handle->cmdBuffer.begin(beginInfo), "Can't begin command buffer");
}

void VulkanCommandList::Close()
{
	VK_CHECK_VOID(m_handle->cmdBuffer.end(),"Can't end command buffer");
}