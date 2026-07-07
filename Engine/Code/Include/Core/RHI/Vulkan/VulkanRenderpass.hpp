#pragma once

#include "RHI/API/Renderpass.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_CORE

class VulkanRenderpass : public Renderpass
{
public:
    VulkanRenderpass() = default;
    virtual ~VulkanRenderpass() override = default;

public:
    static vk::RenderingAttachmentInfo GetRenderingAttachmentInfo(const RenderingAttachment& attachment);
};

END_NAMESPACE_CORE