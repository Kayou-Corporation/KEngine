#include <iostream>

#include "Window/Window.hpp"
#include "Private/Vulkan/VulkanInstance.hpp"
#include "Public/Surface.hpp"
#include "Public/Device.hpp"
#include "Public/Swapchain.hpp"
#include "Public/Buffer.hpp"
#include "Public/Shader.hpp"
#include "Public/Image.hpp"
#include "Public/Renderpass.hpp"
#include "Public/CommandList.hpp"
#include "Public/Syncronisation.hpp"

int main()
{
#ifdef KENGINE_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    Kayou::Core::RefCountPtr<Kayou::Core::Window> window = Kayou::Core::WindowInterface::InitWindow(Kayou::Core::WindowAPI::SDL);

    Kayou::Core::WindowSpecs specs;
    specs.width = 720;
    specs.height = 480;
    specs.name = "KEngine";
    specs.allowResize = true;
    specs.rendererAPI = Kayou::Core::RendererAPI::Vulkan;
    
    window->Create(specs);


    Kayou::Core::RefCountPtr<Kayou::RHI::Instance> instance = Kayou::RHI::RendererInterface::InitRenderer(Kayou::Core::RendererAPI::Vulkan);

    Kayou::RHI::InstanceSpecs test;
    test.window = window;
    test.appVersion = Kayou::RHI::Version(0, 0, 1);
    test.engineVersion = Kayou::RHI::Version(0, 0, 1);
    test.debugLayers = {Kayou::RHI::DebugLayers::Validation };
    
    instance->Create(test);

    Kayou::Core::RefCountPtr<Kayou::RHI::Surface> surface = instance->CreateSurface({ window });

    Kayou::RHI::DeviceSpecs dSpecs;
    dSpecs.gpuType = Kayou::RHI::GpuType::Discrete;
    dSpecs.extensions = {Kayou::RHI::Extensions::Swapchain, Kayou::RHI::Extensions::DynamicRendering, Kayou::RHI::Extensions::ShaderObject };
    dSpecs.queues = {Kayou::RHI::QueueType::Graphics };
    dSpecs.searchPresentQueue = true;
    dSpecs.surface = surface;

    Kayou::Core::RefCountPtr<Kayou::RHI::Device> device = instance->CreateDevice(dSpecs);

    Kayou::RHI::SwapchainSpecs sSpecs;
    sSpecs.surface = surface;
    sSpecs.extent = Kayou::RHI::Extent2D(window->GetHeight(), window->GetWidth());
    sSpecs.imageCount = 2;
    sSpecs.presentMode = Kayou::RHI::PresentMode::Mailbox;
    sSpecs.imageFormat = Kayou::RHI::Format::BGRA8_SRGB;
    sSpecs.isDepthEnable = true;
    sSpecs.depthImageFormat = Kayou::RHI::Format::D32_SFLOAT;

    Kayou::Core::RefCountPtr<Kayou::RHI::Swapchain> swapchain = device->CreateSwapchain(sSpecs);

    //Kayou::RHI::BufferSpecs bufferSpecs{};
    //bufferSpecs.primaryUsage = Kayou::RHI::BufferUsage::Vertex;
    //bufferSpecs.additionalUsages = { Kayou::RHI::BufferUsage::TransferDst };
    //bufferSpecs.size = 65536;
    //bufferSpecs.memoryAccess = Kayou::RHI::MemoryAccess::GPU_Only;
    //bufferSpecs.pipelineStage = Kayou::RHI::PipelineStage::VertexInput;
    //
    //Kayou::Core::RefCountPtr<Kayou::RHI::Buffer> testBuffer = device->CreateBuffer(bufferSpecs);

    std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::Image>> presentationImages = device->CreatePresentationImages(swapchain);

    Kayou::RHI::SwapchainImageSpecs depthImageSpecs; 
    depthImageSpecs.imageType = Kayou::RHI::SwapchainImageType::Depth;
    depthImageSpecs.targetLayout = Kayou::RHI::Layout::DepthStencilAttachment;
    depthImageSpecs.finalLayout = Kayou::RHI::Layout::DepthStencilAttachment;
    depthImageSpecs.type = Kayou::RHI::ImageType::Image2D;
    depthImageSpecs.usages = { Kayou::RHI::ImageUsage::DepthStencilAttachment };
    depthImageSpecs.viewType = Kayou::RHI::ImageViewType::Image2D;
    depthImageSpecs.viewAspect = Kayou::RHI::ImageViewAspect::Depth;

    Kayou::Core::RefCountPtr<Kayou::RHI::Image> depthImage = device->CreateImagesWithSwapchain(depthImageSpecs, swapchain);

    //Kayou::RHI::RenderingAttachment colorAttachment;
    //colorAttachment.

    //Kayou::RHI::ImageSpecs textureImageSpecs;
    //textureImageSpecs.source = Kayou::RHI::ImageSource::Cpu;
    //textureImageSpecs.format = Kayou::RHI::Format::RGBA8_SRGB;
    //textureImageSpecs.targetLayout = Kayou::RHI::Layout::ShaderReadOnly;
    //textureImageSpecs.finalLayout = Kayou::RHI::Layout::ShaderReadOnly;
    //textureImageSpecs.type = Kayou::RHI::ImageType::Image2D;
    //textureImageSpecs.viewType = Kayou::RHI::ImageViewType::Image2D;
    //textureImageSpecs.viewAspect = Kayou::RHI::ImageViewAspect::Color;
    //textureImageSpecs.usages = { Kayou::RHI::ImageUsage::TransferDst, Kayou::RHI::ImageUsage::ShaderSampled };
    //textureImageSpecs.extent = { 1024, 1024, 1};
    //
    //Kayou::Core::RefCountPtr<Kayou::RHI::Image> textureImage = device->CreateImage(textureImageSpecs);
    //
    //Kayou::Core::RefCountPtr<Kayou::RHI::CommandList> commandList = device->GetCommandList(Kayou::RHI::QueueType::Graphics);
    //
    //std::vector<uint32_t> data(1024 * 1024, 0xFF0000FF);
    //
    //commandList->Open();
    //
    //commandList->SetImageData(textureImage, data.data(), data.size() * sizeof(uint32_t));
    //
    //commandList->Close();
    //
    //device->SubmitCommandList(commandList);

    uint32_t windowWidth = window->GetWidth();
    uint32_t windowHeight = window->GetHeight();

    //std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::Fence>> inFlightFences;
    //std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore>> imageAvailablesSemaphore;
    //std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore>> renderFinishedSemaphores;

    //for (uint32_t i = 0; i < swapchain->GetImageCount(); ++i)
    //{
    //    Kayou::Core::RefCountPtr<Kayou::RHI::Fence> fence = device->CreateFence();
    //    inFlightFences.push_back(fence);
    //
    //    Kayou::RHI::SemaphoreSpecs semaphoreSpecs;
    //    semaphoreSpecs.type = Kayou::RHI::SemaphoreType::Timeline;
    //
    //    Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore> semaphore = device->CreateSemaphore(semaphoreSpecs);
    //    renderFinishedSemaphores.push_back(semaphore);
    //
    //    Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore> availableSemaphore = device->CreateSemaphore(semaphoreSpecs);
    //    imageAvailablesSemaphore.push_back(availableSemaphore);
    //}

    // Timeline
    Kayou::RHI::SemaphoreSpecs timelineSpecs;
    timelineSpecs.type = Kayou::RHI::SemaphoreType::Timeline;
    Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore> frameTimelineSemaphore = device->CreateSemaphore(timelineSpecs);

    Kayou::Core::RefCountPtr<Kayou::RHI::Buffer> testBuffer = device->CreateBuffer(bufferSpecs);

    Kayou::Core::RefCountPtr<Kayou::RHI::Shader> baseVert = device->CreateShader("base.vert", Kayou::RHI::ShaderStage::Vertex);
    Kayou::Core::RefCountPtr<Kayou::RHI::Shader> unlitFrag = device->CreateShader("unlit.frag", Kayou::RHI::ShaderStage::Fragment);
    // Binary 
    std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore>> imageAvailablesSemaphores;
    std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore>> renderFinishedSemaphores;

    for (uint32_t i = 0; i < swapchain->GetImageCount(); ++i) 
    {
        Kayou::RHI::SemaphoreSpecs binarySpecs{};
        binarySpecs.type = Kayou::RHI::SemaphoreType::Binary;

        Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore> imageAvailableSemaphore = device->CreateSemaphore(binarySpecs);
        Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore> renderFinishedSemaphore = device->CreateSemaphore(binarySpecs);

        imageAvailablesSemaphores.push_back(imageAvailableSemaphore);
        renderFinishedSemaphores.push_back(renderFinishedSemaphore);
    }

    uint64_t frameCounter = 0;
    
    while (!window->ShouldClose())
    {
        window->PollEvents();

        uint32_t maxFramesInFlight = swapchain->GetImageCount();
        uint32_t syncIndex = frameCounter % maxFramesInFlight;

        if (frameCounter >= maxFramesInFlight)
        {
            uint64_t waitValue = frameCounter - maxFramesInFlight + 1;
            device->WaitForSemaphore(frameTimelineSemaphore, waitValue);
        }

        uint32_t imageIndex = device->AcquirreNextImage(swapchain, imageAvailablesSemaphores[syncIndex]);

        device->RunGarbageCollector();

        Kayou::RHI::RenderingAttachment colorAttachment;
        colorAttachment.image = presentationImages[imageIndex];
        colorAttachment.layout = Kayou::RHI::Layout::ColorAttachment;
        colorAttachment.loadOp = Kayou::RHI::LoadOp::Clear;
        colorAttachment.storeOp = Kayou::RHI::StoreOp::Store;
        colorAttachment.clearValue = Kayou::RHI::ClearValue(1.0f, 0.0f, 0.0f, 1.0f);

        Kayou::RHI::RenderingAttachment depthAttachment;
        depthAttachment.image = depthImage;
        depthAttachment.layout = Kayou::RHI::Layout::DepthStencilAttachment;
        depthAttachment.loadOp = Kayou::RHI::LoadOp::Clear;
        depthAttachment.storeOp = Kayou::RHI::StoreOp::Store;
        depthAttachment.clearValue = Kayou::RHI::ClearValue(1.0f, 0.f, 0.f, 0.f);

        Kayou::RHI::RenderingInfo renderingInfo;
        renderingInfo.offset = { 0, 0 };
        renderingInfo.extent = { windowWidth, windowHeight };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.colorAttachments = { colorAttachment };
        renderingInfo.depthAttachment = depthAttachment;

        auto commandList = device->GetCommandList(Kayou::RHI::QueueType::Graphics);
        commandList->Open();

        commandList->TransitionImageLayout(presentationImages[imageIndex], Kayou::RHI::Layout::ColorAttachment);

        commandList->TransitionImageLayout(depthImage, Kayou::RHI::Layout::DepthStencilAttachment);

        commandList->BeginRendering(renderingInfo);

        commandList->EndRendering();

        commandList->TransitionImageLayout(presentationImages[imageIndex], Kayou::RHI::Layout::Present);

        commandList->Close();

        uint64_t signalValue = frameCounter + 1;

        Kayou::RHI::SubmitInfo submitInfo;
        submitInfo.waitSemaphores = { imageAvailablesSemaphores[syncIndex] };
        submitInfo.waitSemaphoresValues = { 0 };
        submitInfo.signalSemaphores = { frameTimelineSemaphore, renderFinishedSemaphores[syncIndex] };
        submitInfo.signalSemaphoresValues = { signalValue, 0 };
        submitInfo.stage = Kayou::RHI::PipelineStage::ColorOutput;

        device->SubmitCommandList(commandList, submitInfo);

        Kayou::RHI::PresentInfo presentInfo;
        presentInfo.waitSemaphores = { renderFinishedSemaphores[syncIndex] };
        presentInfo.swapchain = swapchain;
        presentInfo.imageIndex = imageIndex;

        device->Present(presentInfo);

        frameCounter++;
    }

	device->DestroyShader(unlitFrag);
	device->DestroyShader(baseVert);
    
    device->WaitIdle();
    //device->DestroyBuffer(testBuffer);

    for (uint32_t i = 0; i < swapchain->GetImageCount(); ++i)
    {
        // delete fence & semaphore
        device->DestroySemaphore(imageAvailablesSemaphores[i]);
        device->DestroySemaphore(renderFinishedSemaphores[i]);
    }
    device->DestroySemaphore(frameTimelineSemaphore);

    device->DestroyPresentationImages(presentationImages);

    device->DestroyImage(depthImage);

    //device->DestroyImage(textureImage);

    device->DestroySwapchain(swapchain);

    instance->DestroyDevice(device);

    instance->DestroySurface(surface);

    instance->Destroy();

    window->Destroy();

    return 0;
}
