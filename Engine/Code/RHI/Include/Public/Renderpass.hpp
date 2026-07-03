#pragma once

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct RenderingAttachment
{
	Core::RefCountPtr<Image> image;
	Layout layout;

	LoadOp loadOp;
	StoreOp storeOp;
	
	ClearValue clearValueColor;
	ClearValue clearValueDepth;
}; 

struct RenderingInfo
{
	Offset2D offset;
	Extent2D extent;
	uint32_t layerCount;

	uint32_t colorAttachmentCount;
	std::vector<RenderingAttachment> colorAttachments;
	RenderingAttachment depthAttachment;
	//RenderingAttachment stencilAttachment;
};

class Renderpass : public virtual Core::IResource
{
public:
	virtual ~Renderpass() = default;
};

//VkRenderingAttachmentInfo colorAttachment{};
//colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
//colorAttachment.imageView = colorImageView;
//colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//colorAttachment.clearValue.color = { {0.1f, 0.1f, 0.2f, 1.0f} };
//
//VkRenderingAttachmentInfo depthAttachment{};
//depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
//depthAttachment.imageView = depthImageView;
//depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
//depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//depthAttachment.clearValue.depthStencil = { 1.0f, 0 };
//
//VkRenderingInfo renderingInfo{};
//renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
//renderingInfo.renderArea.offset = { 0, 0 };
//renderingInfo.renderArea.extent = extent;
//renderingInfo.layerCount = 1;
//
//renderingInfo.colorAttachmentCount = 1;
//renderingInfo.pColorAttachments = &colorAttachment;
//renderingInfo.pDepthAttachment = &depthAttachment;
//renderingInfo.pStencilAttachment = nullptr;
//
//vkCmdBeginRendering(cmdBuffer, &renderingInfo);

END_NAMESPACE_RHI