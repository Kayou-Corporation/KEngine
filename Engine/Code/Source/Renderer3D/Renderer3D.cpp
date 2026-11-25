#include "Renderer3D.hpp"

#include "KEngine.hpp"
#include "RHI/API/Device.hpp"
#include "RHI/API/GraphicsPipeline.hpp"
#include "GraphicPass.hpp"
#include "RHI/API/DescriptorPool.hpp"
#include "RHI/API/DescriptorSetArray.hpp"
#include "RHI/API/Image.hpp"
#include "RHI/API/Syncronisation.hpp"

BEGIN_NAMESPACE_3DRENDERER

// Constructor (protected)
Renderer3D::Renderer3D(Core::SwapchainHandle targetSwapchain)
{
    // Setup properties
    m_targetSwapchain = targetSwapchain;
    m_frameInFlight = targetSwapchain->GetImageCount();

    // local device reference
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    // Target images
    m_renderTargetImages = device->CreatePresentationImages(targetSwapchain);
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

    m_renderTargetDepthImages = device->CreateImagesWithSwapchain(depthImageSpecs, m_targetSwapchain);

    // Syncronisation
    CreateSyncronisationObjects();

    // Global resources BP:
    CreateGlobalLayouts();

    // Actual global resources :
    CreateGlobalRessourcesDescriptors();
}

// Destructor (public)
Renderer3D::~Renderer3D()
{
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    device->WaitIdle();

    // Destroy presentation images
    device->DestroyPresentationImages(m_renderTargetImages);
    device->DestroyImage(m_renderTargetDepthImages);

    DestroySyncronisationObjects();

    DestroyGlobalRessourcesDescriptors();

    DestroyGlobalLayouts();

}

void Renderer3D::Draw()
{
    // local device reference
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();
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
    //Core::Extent3D drawExtent = m_renderTargetImages[imageIndex]->GetExtent();
    //Core::Extent2D drawOffset = Core::Extent2D  (0, 0);

    // Global ressources :
    //for (uint32_t i = 0; i < m_globalDescriptorSets.size(); ++i)
    //{
    //    device->BindDescriptorSet(globalPipelineLayout, "drawData", drawDataDescriptorSet, Core::PipelineBindPoint::Graphics);
    //}

    Core::CommandListHandle mainCommandList = device->GetCommandList(Core::QueueType::Graphics);

    for (uint32_t i = 0; i < m_graphicPasses.size(); ++i)
    {
        ExecuteGraphicPass(m_graphicPasses[i], imageIndex);
    }


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

void Renderer3D::CreateSyncronisationObjects()
{
    // Get the device
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    // Syncronisation
    m_renderTargetAvailableSemaphores.resize(m_frameInFlight);
    m_renderFinishedSemaphores.resize(m_frameInFlight);

    Core::SemaphoreSpecs semaphoreSpecs;
    semaphoreSpecs.type = Core::SemaphoreType::Binary;
    for (uint64_t i = 0; i < m_frameInFlight; i++)
    {
        m_renderTargetAvailableSemaphores[i] = device->CreateSemaphore(semaphoreSpecs);
        m_renderFinishedSemaphores[i] = device->CreateSemaphore(semaphoreSpecs);
    }

    Core::SemaphoreSpecs frameTimelineSemaphoreSpecs;
    frameTimelineSemaphoreSpecs.type = Core::SemaphoreType::Timeline;
    m_frameTimelineSemaphore = device->CreateSemaphore(frameTimelineSemaphoreSpecs);
}

void Renderer3D::DestroySyncronisationObjects()
{
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    // Destroy syncronisation objects
    for (uint64_t i = 0; i < m_frameInFlight; i++)
    {
        device->DestroySemaphore(m_renderTargetAvailableSemaphores[i]);
        device->DestroySemaphore(m_renderFinishedSemaphores[i]);
    }

    device->DestroySemaphore(m_frameTimelineSemaphore);
}

void Renderer3D::CreateGlobalLayouts()
{
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    m_globalLayoutShader = device->CreateShader("globalLayout", Core::ShaderStage::Vertex, true, true);
    m_globalLayoutDescriptorSets = device->CreateDescriptorSetsLayouts(m_globalLayoutShader);
    m_globalLayoutPushConstants = device->CreatePushConstantLayout(m_globalLayoutShader);
    m_globalLayout = device->CreatePipelineLayout(m_globalLayoutDescriptorSets, {m_globalLayoutPushConstants});
}

void Renderer3D::DestroyGlobalLayouts()
{
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    for (uint32_t i = 0; i < m_globalLayoutDescriptorSets.size(); ++i)
    {
        device->DestroyDescriptorSetLayout(m_globalLayoutDescriptorSets[i]);
    }
    device->DestroyPipelineLayout(m_globalLayout);
    device->DestroyShader(m_globalLayoutShader);
}

void Renderer3D::CreateGlobalRessourcesDescriptors()
{
    std::vector<Core::DescriptorSetLayoutHandle> descriptorSetArrayLayouts;
    std::vector<Core::DescriptorSetLayoutHandle> descriptorSetLayouts;
    for (uint32_t i = 0; i < m_globalLayoutDescriptorSets.size(); ++i)
    {
        Core::DescriptorSetLayoutHandle layout = m_globalLayoutDescriptorSets[i];

        bool asArray = layout->GetAsArray();

        if (asArray)
        {
            descriptorSetArrayLayouts.push_back(layout);
        }
        else
        {
            descriptorSetLayouts.push_back(layout);
        }
    }

    CreateGlobalRessourcesDescriptorType(descriptorSetArrayLayouts, true);
    CreateGlobalRessourcesDescriptorType(descriptorSetLayouts, false);
}

void Renderer3D::DestroyGlobalRessourcesDescriptors()
{
    // Get the device
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    for (uint32_t i = 0; i < m_globalDescriptorSetsArray.size(); ++i)
    {
        device->DestroyDescriptorSetArray(m_globalDescriptorSetsArray[i], m_globalDescriptorSetsArrayPool[i]);
    }
    for (uint32_t i = 0; i < m_globalDescriptorSetsArrayPool.size(); ++i)
    {
        device->DestroyDescriptorPool(m_globalDescriptorSetsArrayPool[i]);
    }

    for (uint32_t i = 0; i < m_globalDescriptorSets.size(); ++i)
    {
        device->DestroyDescriptorSet(m_globalDescriptorSets[i], m_globalDescriptorSetsPool);
    }
    device->DestroyDescriptorPool(m_globalDescriptorSetsPool);
}

void Renderer3D::CreateGlobalRessourcesDescriptorType(const std::vector<Core::DescriptorSetLayoutHandle>& layouts, bool array)
{
    if (layouts.empty())
        return;

    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();
    if (array)
    {
        std::unordered_map<Core::DescriptorType, uint32_t> descriptors;
        for (uint32_t i = 0; i < layouts.size(); ++i)
        {
            Core::DescriptorSetLayoutHandle layout = layouts[i];
            const std::vector<Core::Binding> &bindings = layout->GetShaderBindings();
            for (uint32_t j = 0; j < bindings.size(); ++j)
            {
                const Core::Binding &binding = bindings[j];
                // TODO : Add translation funciton
                descriptors[binding.type] += MAX_DESCRIPTORSETARRAY_SIZE;
            }

            Core::DescriptorPoolSpecs poolSpecs;
            poolSpecs.maxDescriptorSetsCount = static_cast<uint32_t>(layouts.size());
            poolSpecs.descriptors = descriptors;
            poolSpecs.poolForArray = true;

            Core::DescriptorPoolHandle pool = device->CreateDescriptorPool(poolSpecs);
            if (pool == nullptr)
            {
                spdlog::error("Renderer3D::CreateGlobalRessourcesDescriptors : Failed to create descriptor pool for global ressources array descriptors");
                return;
            }

            m_globalDescriptorSetsArrayPool.push_back(pool);
        }

        for (uint32_t i = 0; i < layouts.size(); ++i)
        {
            Core::DescriptorSetArrayHandle arraySet = device->CreateDescriptorSetArray(layouts[i], m_globalDescriptorSetsArrayPool[i], MAX_DESCRIPTORSETARRAY_SIZE / 2);
            m_globalDescriptorSetsArray.push_back(arraySet);
        }
        return;
    }

    std::unordered_map<Core::DescriptorType, uint32_t> descriptors;
    for (uint32_t i = 0; i < layouts.size(); ++i)
    {
        Core::DescriptorSetLayoutHandle layout = layouts[i];
        const std::vector<Core::Binding> &bindings = layout->GetShaderBindings();
        for (uint32_t j = 0; j < bindings.size(); ++j)
        {
            const Core::Binding &binding = bindings[j];
            descriptors[binding.type] += binding.count;
        }
    }

    Core::DescriptorPoolSpecs poolSpecs;
    poolSpecs.maxDescriptorSetsCount = static_cast<uint32_t>(layouts.size());
    poolSpecs.descriptors = descriptors;
    poolSpecs.poolForArray = false;

    m_globalDescriptorSetsPool = device->CreateDescriptorPool(poolSpecs);

    if (m_globalDescriptorSetsPool == nullptr)
    {
        spdlog::error("Renderer3D::CreateGlobalRessourcesDescriptors : Failed to create descriptor pool for global ressources descriptors");
        return;
    }

    for (uint32_t i = 0; i < layouts.size(); ++i)
    {
        Core::DescriptorSetHandle descriptorSet = device->CreateDescriptorSet(layouts[i], m_globalDescriptorSetsPool);
        m_globalDescriptorSets.push_back(descriptorSet);
    }
}

void Renderer3D::ExecuteGraphicPass(GraphicPass &pass, uint32_t imageIndex)
{
    Core::Extent3D drawExtent = m_renderTargetImages[imageIndex]->GetExtent();
    Core::Offset2D drawOffset = Core::Offset2D(0, 0);

    pass.BeginPass(drawOffset, Core::Extent2D(drawExtent.x, drawExtent.y), m_renderTargetImages[imageIndex], m_renderTargetDepthImages);



    pass.EndPass();
}

END_NAMESPACE_3DRENDERER
