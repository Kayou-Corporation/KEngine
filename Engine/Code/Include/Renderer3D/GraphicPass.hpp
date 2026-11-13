#pragma once

#include "Renderer3DModule.hpp"

#include "RHI/API/Renderpass.hpp"
#include <string>

BEGIN_NAMESPACE_3DRENDERER

struct DrawBinding
{
    std::string name;
    Core::PipelineHandle pipeline;
};

class GraphicPass
{
public:
    GraphicPass() = delete;
    GraphicPass(std::string name);
    ~GraphicPass() = default;

    void BeginPass(Core::Offset2D drawOffset, Core::Extent2D drawExtent, Core::ImageHandle colorImageTarget, Core::ImageHandle depthImageTarget);
    void EndPass();

    void PushDrawStep(const DrawBinding& drawBinding) {m_drawSteps.push_back(drawBinding);}

    Core::CommandListHandle GetMainCommandList() const;

private:
    DrawBinding FindFirstDrawBinding();

    // Base attachments
    Core::RenderingAttachment colorAttachments;
    Core::RenderingAttachment depthAttachment;

    // Associated pass :
    std::vector<DrawBinding> m_drawStep;

    // Execution :
    Core::CommandListHandle m_mainCommandList;
    Core::SemaphoreHandle m_mainSemaphore;

    std::string m_name;

    std::vector<DrawBinding> m_drawSteps;
};

END_NAMESPACE_3DRENDERER