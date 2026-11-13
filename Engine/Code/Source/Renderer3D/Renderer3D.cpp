#include "Renderer3D/Renderer3D.hpp"

#include "Engine/KEngine.hpp"
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

    m_renderTargerDepthImages = localDevice->CreateImagesWithSwapchain(depthImageSpecs, m_targetSwapchain);

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
    m_globalShader = localDevice->CreateShader("globalLayout", Core::ShaderStage::Vertex, true, true);
    m_globalDescriptorSetsLayouts = localDevice->CreateDescriptorSetsLayouts(m_globalShader);
    m_globalPipelineLayout = localDevice->CreatePipelineLayout(m_globalDescriptorSetsLayouts, {});

    for (uint32_t i = 0; i < m_globalDescriptorSetsLayouts.size(); ++i)
    {
        Core::DescriptorSetHandle globalDescriptorSet = localDevice->CreateDescriptorSet(m_globalDescriptorSetsLayouts[i]);
        m_globalDescriptorSets.push_back(globalDescriptorSet);
    }

    Core::ShaderHandle baseVert = localDevice->CreateShader("base.vert", Core::ShaderStage::Vertex, false, true);
    Core::ShaderHandle unlitFrag = localDevice->CreateShader("unlit.frag", Core::ShaderStage::Fragment, false, true);
    m_shadersLibrary.push_back(baseVert);
    m_shadersLibrary.push_back(unlitFrag);
}

Renderer3D::~Renderer3D()
{
    // local device reference
    Core::DeviceHandle localDevice = Core::KEngine::GetDevice();

    localDevice->WaitIdle();

    localDevice->DestroyPresentationImages(m_renderTargetImages);
    localDevice->DestroyImage(m_renderTargerDepthImages);

    for (uint64_t i = 0; i < m_frameInFlight; i++)
    {
        localDevice->DestroySemaphore(m_renderTargetAvailableSemaphores[i]);
        localDevice->DestroySemaphore(m_renderFinishedSemaphores[i]);
    }

    localDevice->DestroySemaphore(m_frameTimelineSemaphore);

    for (uint32_t i = 0; i < m_globalDescriptorSets.size(); ++i)
    {
        localDevice->DestroyDescriptorSet(m_globalDescriptorSets[i]);
    }

    // TODO : Destroy graphic pass

    // Destroy global pipeline layout
    localDevice->DestroyPipelineLayout(m_globalPipelineLayout);
    localDevice->DestroyDescriptorSetsLayouts(m_globalDescriptorSetsLayouts);
    localDevice->DestroyShader(m_globalShader);

    for (uint32_t i = 0; i < m_shadersLibrary.size(); ++i)
    {
        localDevice->DestroyShader(m_shadersLibrary[i]);
    }

}

// ACTUAL RENDERER IMPLEMENTATION
void Renderer3D::SetupRenderer()
{
    // local device reference :
    Core::DeviceHandle localDevice = Core::KEngine::GetDevice();

    Core::KSharedPtr<GraphicPass> unlitPass = Core::MakeShared<GraphicPass>("UnlitPass");

    // Base unlit pipeline
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
    unlitPipelineSpecs.blendColor = false;
    unlitPipelineSpecs.depthTest = true;
    unlitPipelineSpecs.depthWrite = true;
    unlitPipelineSpecs.depthCompare = Core::CompareOp::LessOrEqual;
    unlitPipelineSpecs.dynamicStates = { Core::DynamicState::ViewPort, Core::DynamicState::Scissor };
    unlitPipelineSpecs.topology = Core::PrimitiveTopology::TriangleList;
    unlitPipelineSpecs.shaders = m_shadersLibrary;
    unlitPipelineSpecs.pipelineLayout = m_globalPipelineLayout;
    Core::PipelineHandle unlitPipeline = localDevice->CreateGraphicsPipeline(unlitPipelineSpecs);

    DrawBinding unlitDrawBinding;
    unlitDrawBinding.name = "UnlitPipeline";
    unlitDrawBinding.pipeline = unlitPipeline;
    unlitPass->PushDrawStep(unlitDrawBinding);

    m_graphicPasses.push_back(unlitPass);
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
    for (uint32_t i = 0; i < m_graphicPasses.size(); ++i)
    {
        m_graphicPasses[i].BeginPass(drawOffset, Core::Extent2D(drawExtent.x, drawExtent.y), m_renderTargetImages[imageIndex], m_renderTargerDepthImages);
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

    pass.BeginPass(drawOffset, Core::Extent2D(drawExtent.x, drawExtent.y), m_renderTargetImages[imageIndex], m_renderTargerDepthImages);



    pass.EndPass();
}

END_NAMESPACE_3DRENDERER
