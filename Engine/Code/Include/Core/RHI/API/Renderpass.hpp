#pragma once

#include "RHI/API/RHI.hpp"
#include <vector>
BEGIN_NAMESPACE_CORE

struct RenderingAttachment
{
	ImageHandle image;
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

class Renderpass
{
public:
	virtual ~Renderpass() = default;
};

END_NAMESPACE_CORE