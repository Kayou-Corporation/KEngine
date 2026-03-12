#include "Private/Vulkan/VulkanCommandList.hpp"

#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanDevice.hpp"

BEGIN_NAMESPACE_RHI

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

void VulkanCommandList::SetBufferData(Core::RefCountPtr<Buffer> buffer, void* data, uint32_t size, uint32_t offset, Core::RefCountPtr<Device> device)
{
	ASSERT((size + offset) <= buffer->GetSize(), "Data is too large");

	if (buffer->GetIsGpuOnly())
	{

	}
	else
	{

	}
}

END_NAMESPACE_RHI