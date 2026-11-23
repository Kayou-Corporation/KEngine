#include "Renderer3D/GraphicPass.hpp"

#include "Engine/KEngine.hpp"
#include "RHI/API/CommandList.hpp"
#include "RHI/API/Semaphore.hpp"

#include "spdlog/spdlog.h"

BEGIN_NAMESPACE_3DRENDERER

GraphicPass::GraphicPass(std::string name)
: m_name(name)
{

}

void GraphicPass::BeginPass(Core::Offset2D drawOffset, Core::Extent2D drawExtent, Core::ImageHandle colorImageTargets, Core::ImageHandle depthImageTarget)
{
    depthAttachment.image = depthImageTarget;

    colorAttachments.image = colorImageTargets;

    Core::RenderingInfo renderingInfo;
    renderingInfo.offset = drawOffset;
    renderingInfo.extent = drawExtent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.colorAttachments = {colorAttachments};
    renderingInfo.depthAttachment = depthAttachment;

    if (!m_executionCommandList->GetIsOpen())
    {
        m_executionCommandList->Open();
    }

    m_executionCommandList->BeginRendering(renderingInfo);
}


void GraphicPass::EndPass()
{
    m_executionCommandList->EndRendering();
}

void GraphicPass::GiveExecutionCommandList(Core::CommandListHandle commandList)
{
    if (commandList == nullptr)
    {
        spdlog::error("GraphicPass::SetMainCommandList : commandList is nullptr");
        return;
    }

    if (m_executionCommandList != nullptr)
    {
        if (m_executionCommandList->GetIsOpen())
        {
            spdlog::error("GraphicPass::SetMainCommandList : Cannot change execution commandlist while the current one is open");
            return;
        }
    }

    m_executionCommandList = commandList;
}

void GraphicPass::ResetExeuctionCommandLIst() const
{
    m_executionCommandList = nullptr;
}

END_NAMESPACE_3DRENDERER
