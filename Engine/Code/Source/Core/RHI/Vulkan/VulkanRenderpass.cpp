#include "RHI/Vulkan/VulkanRenderpass.hpp"

#include "RHI/Vulkan/VulkanTranslate.hpp"
#include "RHI/Vulkan/VulkanImage.hpp"

BEGIN_NAMESPACE_RHI

vk::RenderingAttachmentInfo VulkanRenderpass::GetRenderingAttachmentInfo(const RenderingAttachment& attachment)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = attachment.image.CastAs<VulkanImage>();

	vk::ClearValue clearValue;
	if (attachment.layout == Layout::DepthStencilAttachment)
	{
		vk::ClearDepthStencilValue depthStencilValue(attachment.clearValueDepth.x, 0);
		clearValue.depthStencil = depthStencilValue;
	}
	else
	{
		vk::ClearColorValue colorValue = vk::ClearColorValue(std::array<float, 4>{attachment.clearValueColor.x, attachment.clearValueColor.y, attachment.clearValueColor.z, attachment.clearValueColor.a});
		clearValue.color = colorValue;
	}

	vk::RenderingAttachmentInfo renderingAttachment{};

	renderingAttachment.imageView = RHIVulkanImage->GetHandleView();
	renderingAttachment.imageLayout = TranslateToVulkan(attachment.layout);
	renderingAttachment.loadOp = TranslateToVulkan(attachment.loadOp);
	renderingAttachment.storeOp = TranslateToVulkan(attachment.storeOp);
	renderingAttachment.clearValue = clearValue;

	return renderingAttachment;
}

END_NAMESPACE_RHI