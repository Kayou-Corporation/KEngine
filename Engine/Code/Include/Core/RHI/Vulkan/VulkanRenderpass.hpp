#pragma once

#include "Public/Renderpass.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanRenderpass : public Renderpass
{
public:
    VulkanRenderpass() = default;
    virtual ~VulkanRenderpass() override = default;

public:
    static vk::RenderingAttachmentInfo GetRenderingAttachmentInfo(const RenderingAttachment& attachment);
};

END_NAMESPACE_RHI