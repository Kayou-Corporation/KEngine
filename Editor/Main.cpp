#include <iostream>

#include "Window/Window.hpp"
#include "Private/Vulkan/VulkanInstance.hpp"
#include "Public/Surface.hpp"
#include "Public/Device.hpp"
#include "Public/Swapchain.hpp"
#include "Public/Buffer.hpp"
#include "Public/Shader.hpp"

int main()
{
#ifdef KENGINE_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    Kayou::RHI::ShaderCompiler compiler{};

    compiler.Initialize();

    compiler.Load("Engine/Assets/Shaders/hello-world.compute.slang", Kayou::RHI::ShaderType::Compute);

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

    Kayou::RHI::BufferSpecs bufferSpecs{};
    bufferSpecs.primaryUsage = Kayou::RHI::BufferUsage::Vertex;
    bufferSpecs.additionalUsages = { Kayou::RHI::BufferUsage::TransferDst };
    bufferSpecs.size = 65536;
    bufferSpecs.memoryAccess = Kayou::RHI::MemoryAccess::GPU_Only;
    bufferSpecs.pipelineStage = Kayou::RHI::PipelineStage::VertexInput;

    Kayou::Core::RefCountPtr<Kayou::RHI::Buffer> testBuffer = device->CreateBuffer(bufferSpecs);
    
    while (!window->ShouldClose())
    {
        window->PollEvents();
    }
    
    device->DestroyBuffer(testBuffer);

    device->DestroySwapchain(swapchain);

    instance->DestroyDevice(device);

    instance->DestroySurface(surface);

    instance->Destroy();

    window->Destroy();

    return 0;
}
