#include "Private/Vulkan/VulkanRenderpass.hpp"

#include "Private/Vulkan/VulkanTranslate.hpp"
#include "Private/Vulkan/VulkanImage.hpp"

BEGIN_NAMESPACE_RHI

vk::RenderingAttachmentInfo VulkanRenderpass::GetRenderingAttachmentInfo(const RenderingAttachment& attachment)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = attachment.image.CastAs<VulkanImage>();

	vk::RenderingAttachmentInfo renderingAttachment{};

	renderingAttachment.imageView = RHIVulkanImage->GetHandleView();
	renderingAttachment.imageLayout = TranslateToVulkan(attachment.layout);
	renderingAttachment.loadOp = TranslateToVulkan(attachment.loadOp);
	renderingAttachment.storeOp = TranslateToVulkan(attachment.storeOp);
	renderingAttachment.clearValue = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{attachment.clearValue.x, attachment.clearValue.y, attachment.clearValue.z, attachment.clearValue.depthStencil}));

	return renderingAttachment;
}

END_NAMESPACE_RHI