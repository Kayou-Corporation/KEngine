#include "GraphicPass.hpp"

//#include "KEngine.hpp"
//#include "RHI/API/CommandList.hpp"
//#include "RHI/API/GraphicsPipeline.hpp"
//#include "RHI/API/Syncronisation.hpp"
//
//#include "spdlog/spdlog.h"
//
//BEGIN_NAMESPACE_3DRENDERER
//
//GraphicPass::GraphicPass(const GraphicPassDesc& desc)
//: m_name(desc.name)
//, m_colorAttachments(desc.colorAttachments)
//, m_depthAttachment(desc.depthAttachment)
//, m_descriptorSetLayouts(desc.descriptorSetLayouts)
//, m_pushConstantLayouts(desc.pushConstantLayouts)
//, m_steps(desc.steps)
//
//, m_globalLayout(desc.globalLayout)
//, m_globalDescriptorSetLayouts(desc.globalDescriptorSetLayouts)
//, m_globalPushConstantLayout(desc.globalPushConstantLayout)
//{
//    if (m_steps.size() == 0)
//    {
//        spdlog::error("GraphicPass::GraphicPass : No steps provided for the graphic pass: {}", desc.name);
//        return;
//    }
//
//    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();
//
//    for (GraphicPassStep& step : m_steps)
//    {
//        if (step.globalLayoutUsage == DefaultLayoutUsage::UseOnly)
//        {
//            step.pipelineSpecs.pipelineLayout = m_globalLayout;
//            Core::PipelineHandle unlitPipeline = device->CreateGraphicsPipeline(step.pipelineSpecs);
//            m_pipelines.push_back(unlitPipeline);
//        }
//        else
//        {
//            spdlog::error("GraphicPass::GraphicPass : Only DefaultLayoutUsage::UseOnly is currently supported for the graphic pass: {}", desc.name);
//            return;
//        }
//    }
//}
//
//void GraphicPass::BeginPass(Core::Offset2D drawOffset, Core::Extent2D drawExtent, Core::ImageHandle colorImageTargets, Core::ImageHandle depthImageTarget)
//{
//    m_depthAttachment.image = depthImageTarget;
//
//    for (uint32_t i = 0; i < m_colorAttachments.size(); ++i)
//    {
//        m_colorAttachments[i].image = colorImageTargets;
//    }
//
//    Core::RenderingInfo renderingInfo;
//    renderingInfo.offset = drawOffset;
//    renderingInfo.extent = drawExtent;
//    renderingInfo.layerCount = 1;
//    renderingInfo.colorAttachmentCount = 1;
//    renderingInfo.colorAttachments = m_colorAttachments;
//    renderingInfo.depthAttachment = m_depthAttachment;
//
//    if (!m_executionCommandList->GetIsOpen())
//    {
//        m_executionCommandList->Open();
//    }
//
//    m_executionCommandList->BeginRendering(renderingInfo);
//}
//
//
//void GraphicPass::EndPass()
//{
//    m_executionCommandList->EndRendering();
//}
//
//void GraphicPass::GiveExecutionCommandList(Core::CommandListHandle commandList)
//{
//    if (commandList == nullptr)
//    {
//        spdlog::error("GraphicPass::SetMainCommandList : commandList is nullptr");
//        return;
//    }
//
//    if (m_executionCommandList != nullptr)
//    {
//        if (m_executionCommandList->GetIsOpen())
//        {
//            spdlog::error("GraphicPass::SetMainCommandList : Cannot change execution commandlist while the current one is open");
//            return;
//        }
//    }
//
//    m_executionCommandList = commandList;
//}
//
//void GraphicPass::ResetExeuctionCommandLIst()
//{
//    m_executionCommandList.Reset();
//}
//
//END_NAMESPACE_3DRENDERER
