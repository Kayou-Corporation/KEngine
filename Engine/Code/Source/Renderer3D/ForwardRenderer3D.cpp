#include "ForwardRenderer3D.hpp"

#include "KEngine.hpp"
#include "RHI/API/Device.hpp"
#include "RHI/API/GraphicsPipeline.hpp"
#include "RHI/API/CommandList.hpp"
#include "GraphicPass.hpp"
#include "RHI/API/Image.hpp"

BEGIN_NAMESPACE_3DRENDERER

ForwardRenderer3D::ForwardRenderer3D(Core::SwapchainHandle targetSwapchain)
    : Renderer3D(targetSwapchain)
{

}

ForwardRenderer3D::~ForwardRenderer3D()
{
    // Destructor of parent should be automaticely called
}

// ----------- Setup ----------------- //
void ForwardRenderer3D::SetupRenderer()
{
    CreateBasePass();
}

void ForwardRenderer3D::CleanupRenderer()
{
    DestroyBasePass();
}

void ForwardRenderer3D::ResolveRessources()
{
    // If there is 0 command to resolve, to nothing
    if (m_unResolvedDrawCommands.size() == 0)
    {
        return;
    }

    // Reverse loop
    for (uint32_t i = m_unResolvedDrawCommands.size() - 1; i > 0; i--)
    {
        //const DrawCommand& command = m_unResolvedDrawCommands[i];
//
//
        //// Remove resolved object once it's done
        //m_unResolvedDrawCommands.pop_back();
    }
}

// ----------- Creation ----------------- //
void ForwardRenderer3D::CreateBasePass()
{
    // Get the device
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    // Create the base graphic pass
    //GraphicPassDesc basePassDesc;
    //basePassDesc.name = "BasePass";
    //basePassDesc.globalLayout = m_globalLayout;
    //basePassDesc.globalDescriptorSetLayouts = m_globalLayoutDescriptorSets;
    //basePassDesc.globalPushConstantLayout = m_globalLayoutPushConstants;
//
//
    //// Base color :
    //Core::RenderingAttachment colorAttachment;
    //colorAttachment.image = nullptr;
    //colorAttachment.layout = Core::Layout::ColorAttachment;
    //colorAttachment.loadOp = Core::LoadOp::Clear;
    //colorAttachment.storeOp = Core::StoreOp::Store;
    //colorAttachment.clearValueColor = Core::ClearValue(0.1f, 0.1f, 0.1f, 1.0f);
//
    //basePassDesc.colorAttachments.push_back(colorAttachment);
//
//
    //// Base depth :
    //Core::RenderingAttachment depthAttachment;
    //depthAttachment.image = nullptr;
    //depthAttachment.layout = Core::Layout::DepthStencilAttachment;
    //depthAttachment.loadOp = Core::LoadOp::Clear;
    //depthAttachment.storeOp = Core::StoreOp::Store;
    //depthAttachment.clearValueDepth = Core::ClearValue(1.0f, 0.f, 0.f, 0.f);
//
    //basePassDesc.depthAttachment = depthAttachment;
//
//
    //// Base draw step :
    //GraphicPassStep unlitDrawStep;
    //unlitDrawStep.name = "UnlitDrawStep";
    //unlitDrawStep.globalLayoutUsage = DefaultLayoutUsage::UseOnly;

    // Shaders :
    Core::ShaderHandle forwardSharedRenderingVert = device->CreateShader("forwardSharedRendering.vert", Core::ShaderStage::Vertex, false, true);

    Core::ShaderHandle forwardSharedRenderingFrag = device->CreateShader("forwardSharedRendering.frag", Core::ShaderStage::Fragment, false, true);

    Core::GraphicsPipelineColorBlendAttachment unlitPipelineColorBlendAttachement;
    unlitPipelineColorBlendAttachement.blendColor = false;
    unlitPipelineColorBlendAttachement.srcColorBlendFactor = Core::BlendFactor::One;
    unlitPipelineColorBlendAttachement.dstColorBlendFactor = Core::BlendFactor::Zero;
    unlitPipelineColorBlendAttachement.colorBlendOperation = Core::BlendOperation::Add;
    unlitPipelineColorBlendAttachement.colorWriteMask = Core::ColorComponentFlagBits::R | Core::ColorComponentFlagBits::G | Core::ColorComponentFlagBits::B | Core::ColorComponentFlagBits::A;
    Core::GraphicsPipelineSpecs unlitPipelineSpecs;
    unlitPipelineSpecs.colorAttachmentCount = 1;
    unlitPipelineSpecs.colorAttachmentFormats = { m_targetSwapchain->GetColorImageFormat() };
    unlitPipelineSpecs.depthAttachment = m_targetSwapchain->GetDepthImageFormat();
    unlitPipelineSpecs.viewportCount = 1;
    unlitPipelineSpecs.scissorCount = 1;
    unlitPipelineSpecs.lineWidth = 1.f;
    unlitPipelineSpecs.cullmode = Core::CullMode::Back;
    unlitPipelineSpecs.frontFace = Core::FrontFace::CounterClockWise;
    unlitPipelineSpecs.SamplesCount = Core::SampleCount::Count1;
    unlitPipelineSpecs.colorBlendAttachements = { unlitPipelineColorBlendAttachement };
    unlitPipelineSpecs.depthTest = true;
    unlitPipelineSpecs.depthWrite = true;
    unlitPipelineSpecs.depthCompare = Core::CompareOp::LessOrEqual;
    unlitPipelineSpecs.dynamicStates = { Core::DynamicState::ViewPort, Core::DynamicState::Scissor };
    unlitPipelineSpecs.topology = Core::PrimitiveTopology::TriangleList;
    unlitPipelineSpecs.shaders = { forwardSharedRenderingVert , forwardSharedRenderingFrag };

    //unlitDrawStep.pipelineSpecs = unlitPipelineSpecs;

    //GraphicPass basePass = GraphicPass(basePassDesc);

    //m_graphicPasses.push_back(basePass);
}

void ForwardRenderer3D::DestroyBasePass()
{
    // TODO : Implement this
}

END_NAMESPACE_3DRENDERER
