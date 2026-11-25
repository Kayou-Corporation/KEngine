#pragma once

#include "Renderer3DModule.hpp"

#include "RHI/API/Renderpass.hpp"
#include <string>

#include "RHI/API/GraphicsPipeline.hpp"

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
    GraphicPass(const GraphicPassDesc& desc);
    ~GraphicPass() = default;

    void BeginPass(Core::Offset2D drawOffset, Core::Extent2D drawExtent, Core::ImageHandle colorImageTarget, Core::ImageHandle depthImageTarget);
    void EndPass();

    void GiveExecutionCommandList(Core::CommandListHandle commandList);
    void ResetExeuctionCommandLIst();

private:
    // Info
    std::string m_name;

    // Attachments :
    std::vector<Core::RenderingAttachment> m_colorAttachments;
    Core::RenderingAttachment m_depthAttachment;

    // Shared ressources :
    std::vector<Core::DescriptorSetLayoutHandle> m_descriptorSetLayouts;
    std::vector<Core::PushConstantLayoutHandle> m_pushConstantLayouts;

    // Step :
    std::vector<GraphicPassStep> m_steps;
    std::vector<Core::PipelineHandle> m_pipelines;

    // Execution (On Flight) :
    Core::CommandListHandle m_executionCommandList;

    // Global Layout reference (External) :
    Core::PipelineLayoutHandle m_globalLayout;
    std::vector<Core::DescriptorSetLayoutHandle> m_globalDescriptorSetLayouts;
    Core::PushConstantLayoutHandle m_globalPushConstantLayout;


};

END_NAMESPACE_3DRENDERER