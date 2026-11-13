#include "Renderer3D/GraphicPass.hpp"

#include "Engine/KEngine.hpp"
#include "RHI/API/CommandList.hpp"
#include "RHI/API/Semaphore.hpp"

#include "spdlog/spdlog.h"

BEGIN_NAMESPACE_3DRENDERER

GraphicPass::GraphicPass(std::string name)
: m_name(name)
{
    Core::DeviceHandle localDeviceReference = Core::KEngine::GetDevice();
    m_mainCommandList = localDeviceReference->GetCommandList(Core::QueueType::Graphics);

    Core::SemaphoreSpecs mainSemaphoreSpecs;
    mainSemaphoreSpecs.type = Core::SemaphoreType::Binary;
    m_mainSemaphore = localDeviceReference->CreateSemaphore(mainSemaphoreSpecs);
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

    m_mainCommandList->Open();
    m_mainCommandList->BeginRendering(renderingInfo);
}


void GraphicPass::EndPass()
{
    m_mainCommandList->EndRendering();
    m_mainCommandList->Close();
}

void GraphicPass::SetMainCommandList(Core::CommandList commandList)
{
    if (commandList == nullptr)
    {
        spdlog::error("GraphicPass::SetMainCommandList : commandList is nullptr");
        return;
    }

    if (m_mainCommandList != nullptr)
    {
        if (m_mainCommandList->GetIsOpen())
        {
            spdlog::error("GraphicPass::SetMainCommandList : Cannot change main commandlist while the current one is open");
            return;
        }
    }

    m_mainCommandList = commandList;
}

Core::CommandList GraphicPass::GetMainCommandList() const
{
    if (m_mainCommandList == nullptr)
    {
        spdlog::error("GraphicPass::GetMainCommandList : main command list is nullptr");
        return nullptr;
    }

    if (m_mainCommandList->GetIsOpen())
    {
        spdlog::warn("GraphicPass::GetMainCommandList : main command list is open");
    }

    return m_mainCommandList;
}

DrawBinding GraphicPass::FindFirstDrawBinding()
{
    for (const DrawBinding& drawBinding : m_drawSteps)
    {
        if (drawBinding.waitPipeline.empty())
        {
            return drawBinding;
        }
    }

    if (m_drawStep.empty())
    {
        spdlog::error("GraphicPass::FindFirstDrawBinding : No draw steps found in pass {}", m_name);
        return DrawBinding{};
    }

    return m_drawSteps[0];
}

END_NAMESPACE_3DRENDERER
