#pragma once

#include "Renderer3DModule.hpp"

#include "RHI/API/Renderpass.hpp"
#include <string>

BEGIN_NAMESPACE_3DRENDERER

enum DefaultLayoutUsage
{
    UseOnly,
    UseShared,
    UseNone,
};

struct GraphicPassStep
{
    std::string name;

    DefaultLayoutUsage globalLayoutUsage;

    Core::GraphicsPipelineSpecs pipelineSpecs;
    std::vector<std::string> DescriptorSetLayoutNames;
    std::vector<std::string> PushConstantLayoutNames;
};

struct GraphicPassDesc
{
    // Name
    std::string name;

    // Attachments
    std::vector<Core::RenderingAttachment> colorAttachments;
    Core::RenderingAttachment depthAttachment;

    // Shared ressources in pass
    std::vector<Core::DescriptorSetLayoutHandle> descriptorSetLayouts;
    std::vector<Core::PushConstantLayoutHandle> pushConstantLayouts;

    // Sub graphic step (pipeline)
    std::vector<GraphicPassStep> steps;

    // Global layout reference :
    Core::PipelineLayoutHandle globalLayout;
    std::vector<Core::DescriptorSetLayoutHandle> globalDescriptorSetLayouts;
    Core::PushConstantLayoutHandle globalPushConstantLayout;
};

class GraphicPass
{
public:
    GraphicPass() = delete;
    GraphicPass(std::string name);
    ~GraphicPass() = default;

    void BeginPass(Core::Offset2D drawOffset, Core::Extent2D drawExtent, Core::ImageHandle colorImageTarget, Core::ImageHandle depthImageTarget);
    void EndPass();


    void GiveExecutionCommandList(Core::CommandListHandle commandList);
    void ResetExeuctionCommandLIst() const;

private:
    // Info
    std::string m_name;

    // Attachments :
    std::vector<Core::RenderingAttachment> m_colorAttachments;
    Core::RenderingAttachment m_depthAttachment;

    // Global Layout reference :
    Core::PipelineLayoutHandle globalLayout;
    std::vector<Core::DescriptorSetLayoutHandle> globalDescriptorSetLayouts;
    Core::PushConstantLayoutHandle globalPushConstantLayout;

    // Execution (On Flight) :
    Core::CommandListHandle m_executionCommandList;

};

END_NAMESPACE_3DRENDERER