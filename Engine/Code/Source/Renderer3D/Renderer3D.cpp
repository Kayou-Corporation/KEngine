#include "Renderer3D/Renderer3D.hpp"

#include "Engine/KEngine.hpp"
#include "RHI/API/Device.hpp"
#include "RHI/API/GraphicsPipeline.hpp"
#include "Renderer3D/GraphicPass.hpp"
#include "RHI/API/Image.hpp"

BEGIN_NAMESPACE_3DRENDERER

Renderer3D::Renderer3D(Core::SwapchainHandle targetSwapchain)
{
    // Setup properties
    m_targetSwapchain = targetSwapchain;
    m_frameInFlight = targetSwapchain->GetImageCount();

    // local device reference
    Core::DeviceHandle localDevice = Core::KEngine::GetDevice();

    // Target images
    m_renderTargetImages = localDevice->CreatePresentationImages(targetSwapchain);
    if (m_renderTargetImages.size() == 0)
    {
        spdlog::error("Renderer3D::Renderer3D : Failed to create presentation images from swapchain, number of images: {}", m_renderTargetImages.size());
        return;
    }

    Core::SwapchainImageSpecs depthImageSpecs;
    depthImageSpecs.imageType = Core::SwapchainImageType::Depth;
    depthImageSpecs.targetLayout = Core::Layout::DepthStencilAttachment;
    depthImageSpecs.type = Core::ImageType::Image2D;
    depthImageSpecs.usages = { Core::ImageUsage::DepthStencilAttachment };
    depthImageSpecs.viewType = Core::ImageViewType::Image2D;
    depthImageSpecs.viewAspect = Core::ImageViewAspect::Depth;

    m_renderTargetDepthImages = localDevice->CreateImagesWithSwapchain(depthImageSpecs, m_targetSwapchain);

    // Syncronisation
    m_renderTargetAvailableSemaphores.resize(m_frameInFlight);
    m_renderFinishedSemaphores.resize(m_frameInFlight);

    Core::SemaphoreSpecs semaphoreSpecs;
    semaphoreSpecs.type = Core::SemaphoreType::Binary;
    for (uint64_t i = 0; i < m_frameInFlight; i++)
    {
        m_renderTargetAvailableSemaphores[i] = localDevice->CreateSemaphore(semaphoreSpecs);
        m_renderFinishedSemaphores[i] = localDevice->CreateSemaphore(semaphoreSpecs);
    }

    Core::SemaphoreSpecs frameTimelineSemaphoreSpecs;
    frameTimelineSemaphoreSpecs.type = Core::SemaphoreType::Timeline;
    m_frameTimelineSemaphore = localDevice->CreateSemaphore(frameTimelineSemaphoreSpecs);

    // Global resources :
    m_globalLayoutShader = localDevice->CreateShader("globalLayout", Core::ShaderStage::Vertex, true, true);
    m_globalLayoutDescriptorSets = localDevice->CreateDescriptorSetsLayouts(m_globalLayoutShader);
    m_globalLayoutPushConstants = localDevice->CreatePushConstantLayout(m_globalLayoutShader);
    m_globalLayout = localDevice->CreatePipelineLayout(m_globalLayoutDescriptorSets, {m_globalLayoutPushConstants});

    //for (uint32_t i = 0; i < m_globalDescriptorSetsLayouts.size(); ++i)
    //{
    //    Core::DescriptorSetHandle globalDescriptorSet = localDevice->CreateDescriptorSet(m_globalDescriptorSetsLayouts[i]);
    //    m_globalDescriptorSets.push_back(globalDescriptorSet);
    //}
//
    //Core::ShaderHandle baseVert = localDevice->CreateShader("base.vert", Core::ShaderStage::Vertex, false, true);
    //Core::ShaderHandle unlitFrag = localDevice->CreateShader("unlit.frag", Core::ShaderStage::Fragment, false, true);
    //m_shadersLibrary.push_back(baseVert);
    //m_shadersLibrary.push_back(unlitFrag);
}

Renderer3D::~Renderer3D()
{
    // local device reference
    Core::DeviceHandle localDevice = Core::KEngine::GetDevice();

    localDevice->WaitIdle();

    localDevice->DestroyPresentationImages(m_renderTargetImages);
    localDevice->DestroyImage(m_renderTargetDepthImages);

    for (uint64_t i = 0; i < m_frameInFlight; i++)
    {
        localDevice->DestroySemaphore(m_renderTargetAvailableSemaphores[i]);
        localDevice->DestroySemaphore(m_renderFinishedSemaphores[i]);
    }

    localDevice->DestroySemaphore(m_frameTimelineSemaphore);

    //for (uint32_t i = 0; i < m_globalDescriptorSets.size(); ++i)
    //{
    //    localDevice->DestroyDescriptorSet(m_globalDescriptorSets[i]);
    //}

    // TODO : Destroy graphic pass

    // Destroy global pipeline layout
    //localDevice->DestroyPipelineLayout(m_globalPipelineLayout);
    //localDevice->DestroyDescriptorSetsLayouts(m_globalDescriptorSetsLayouts);
    //localDevice->DestroyShader(m_globalShader);
//
    //for (uint32_t i = 0; i < m_shadersLibrary.size(); ++i)
    //{
    //    localDevice->DestroyShader(m_shadersLibrary[i]);
    //}

}

// ACTUAL RENDERER IMPLEMENTATION
void Renderer3D::SetupRenderer()
{
    // local device reference :
    Core::DeviceHandle localDevice = Core::KEngine::GetDevice();

    // Create all shader needed for the renderer (User implemented)
    CreateShaderLibrary();

    //Core::KSharedPtr<GraphicPass> unlitPass = Core::MakeShared<GraphicPass>("UnlitPass");

    // Base unlit pipeline
    Core::GraphicsPipelineSpecs unlitPipelineSpecs;
    unlitPipelineSpecs.colorAttachmentCount = 1;
    unlitPipelineSpecs.colorAttachmentFormats = { swapchain->GetColorImageFormat() };
    unlitPipelineSpecs.depthAttachment = swapchain->GetDepthImageFormat();
    unlitPipelineSpecs.viewportCount = 1;
    unlitPipelineSpecs.scissorCount = 1;
    unlitPipelineSpecs.lineWidth = 1.f;
    unlitPipelineSpecs.cullmode = Core::CullMode::Back;
    unlitPipelineSpecs.frontFace = Core::FrontFace::CounterClockWise;
    unlitPipelineSpecs.SamplesCount = Core::SampleCount::Count1;
    unlitPipelineSpecs.blendColor = false;
    unlitPipelineSpecs.depthTest = true;
    unlitPipelineSpecs.depthWrite = true;
    unlitPipelineSpecs.depthCompare = Core::CompareOp::LessOrEqual;
    unlitPipelineSpecs.dynamicStates = { Core::DynamicState::ViewPort, Core::DynamicState::Scissor };
    unlitPipelineSpecs.topology = Core::PrimitiveTopology::TriangleList;
    unlitPipelineSpecs.shaders = { baseVert , unlitFrag };
    unlitPipelineSpecs.pipelineLayout = globalPipelineLayout;

    //DrawBinding unlitDrawBinding;
    //unlitDrawBinding.name = "UnlitPipeline";
    //unlitDrawBinding.pipeline = unlitPipeline;
    //unlitPass->PushDrawStep(unlitDrawBinding);
//
    //m_graphicPasses.push_back(unlitPass);
}


void Renderer3D::Draw()
{
    // local device reference
    Core::DeviceHandle device = Core::KEngine::GetDevice();
    device->RunGarbageCollector();

    // Basic syncronisation
    uint32_t syncIndex = m_frameCounter % m_frameInFlight;
    if (m_frameCounter >= m_frameInFlight)
    {
        uint64_t waitValue = m_frameCounter - m_frameInFlight + 1;
        device->WaitForSemaphore(m_frameTimelineSemaphore, waitValue);
    }

    uint32_t imageIndex = device->AcquirreNextImage(m_targetSwapchain, m_renderTargetAvailableSemaphores[syncIndex]);

    // Basic infos :
    Core::Extent3D drawExtent = m_renderTargetImages[imageIndex]->GetExtent();
    Core::Extent2D drawOffset = Core::Offset2D(0, 0);

    // Global ressources :
    //for (uint32_t i = 0; i < m_globalDescriptorSets.size(); ++i)
    //{
    //    device->BindDescriptorSet(globalPipelineLayout, "drawData", drawDataDescriptorSet, Core::PipelineBindPoint::Graphics);
    //}

    for (uint32_t i = 0; i < m_graphicPasses.size(); ++i)
    {
        ExecuteGraphicPass(m_graphicPasses[i], imageIndex);
    }

    Core::CommandListHandle commandList = device->GetCommandList(Core::QueueType::Graphics);

    // One main commandBuffer here :
    /*
     * Submit main :
     *      wait -> image available
     *      signal -> frameTimeline, renderFinished
     *
     * Present on screen :
     *      wait -> renderFinished
     *
     */

    m_frameCounter++;
}

void Renderer3D::ExecuteGraphicPass(const GraphicPass &pass, uint32_t imageIndex)
{
    Core::Extent3D drawExtent = m_renderTargetImages[imageIndex]->GetExtent();
    Core::Extent2D drawOffset = Core::Offset2D(0, 0);

    pass.BeginPass(drawOffset, Core::Extent2D(drawExtent.x, drawExtent.y), m_renderTargetImages[imageIndex], m_renderTargetDepthImages);



    pass.EndPass();
}

void Renderer3D::CreateShaderLibrary()
{
    Core::DeviceHandle device = Core::KEngine::GetDevice();

    // Base Unlit :
    Core::ShaderHandle forwardSharedRenderingVert = device->CreateShader("forwardSharedRendering.vert", Core::ShaderStage::Vertex, false, true);
    m_shadersLibrary.push_back(forwardSharedRenderingVert);

    Core::ShaderHandle forwardSharedRenderingFrag = device->CreateShader("forwardSharedRendering.frag", Core::ShaderStage::Fragment, false, true);
    m_shadersLibrary.push_back(forwardSharedRenderingFrag);
}

void Renderer3D::DestroyShaderLibrary()
{
    Core::DeviceHandle device = Core::KEngine::GetDevice();

    for (uint32_t i = 0; i < m_shadersLibrary.size(); ++i)
    {
        device->DestroyShader(m_shadersLibrary[i]);
    }
}

void Renderer3D::CreateBasePass()
{
    // Create the base graphic pass
    GraphicPassDesc basePassDesc;
    basePassDesc.name = "BasePass";
    basePassDesc.globalLayout = m_globalLayout;
    basePassDesc.globalDescriptorSetLayouts = m_globalLayoutDescriptorSets;
    basePassDesc.globalPushConstantLayout = m_globalLayoutPushConstants;


    // Base color :
    Core::RenderingAttachment colorAttachment;
    colorAttachment.image = nullptr;
    colorAttachment.layout = Core::Layout::ColorAttachment;
    colorAttachment.loadOp = Core::LoadOp::Clear;
    colorAttachment.storeOp = Core::StoreOp::Store;
    colorAttachment.clearValueColor = Core::ClearValue(0.1f, 0.1f, 0.1f, 1.0f);
    basePassDesc.colorAttachments.push_back(colorAttachment);


    // Base depth :
    Core::RenderingAttachment depthAttachment;
    depthAttachment.image = nullptr;
    depthAttachment.layout = Core::Layout::DepthStencilAttachment;
    depthAttachment.loadOp = Core::LoadOp::Clear;
    depthAttachment.storeOp = Core::StoreOp::Store;
    depthAttachment.clearValueDepth = Core::ClearValue(1.0f, 0.f, 0.f, 0.f);
    basePassDesc.depthAttachment = depthAttachment;

    // Base draw step :
    GraphicPassStep unlitDrawStep;
    unlitDrawStep.name = "UnlitDrawStep";
    unlitDrawStep.globalLayoutUsage = DefaultLayoutUsage::UseOnly;

    Core::GraphicsPipelineSpecs unlitPipelineSpecs;



    //basePassDesc.drawSteps.push_back(unlitDrawStep);

}

void Renderer3D::DestroyBasePass()
{
    // Destroy the base graphic pass
}

END_NAMESPACE_3DRENDERER
