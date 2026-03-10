#include <iostream>

#include "Window/Window.hpp"
#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"
#include "Core/RHI/Public/Surface.hpp"
#include "Core/RHI/Public/Device.hpp"
#include "Core/RHI/Public/Swapchain.hpp"
#include "Core/RHI/Public/Buffer.hpp"

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


    Kayou::Core::RefCountPtr<Kayou::Core::Instance> instance = Kayou::Core::RendererInterface::InitRenderer(Kayou::Core::RendererAPI::Vulkan);

    Kayou::Core::InstanceSpecs test;
    test.window = window;
    test.appVersion = Kayou::Core::Version(0, 0, 1);
    test.engineVersion = Kayou::Core::Version(0, 0, 1);
    test.debugLayers = {Kayou::Core::DebugLayers::Validation };
    
    instance->Create(test);

    Kayou::Core::RefCountPtr<Kayou::Core::Surface> surface = instance->CreateSurface({ window });

    Kayou::Core::DeviceSpecs dSpecs;
    dSpecs.gpuType = Kayou::Core::GpuType::Discrete;
    dSpecs.extensions = {Kayou::Core::Extensions::Swapchain, Kayou::Core::Extensions::DynamicRendering, Kayou::Core::Extensions::ShaderObject };
    dSpecs.queues = {Kayou::Core::QueueType::Graphics };
    dSpecs.searchPresentQueue = true;
    dSpecs.surface = surface;

    Kayou::Core::RefCountPtr<Kayou::Core::Device> device = instance->CreateDevice(dSpecs);

    Kayou::Core::SwapchainSpecs sSpecs;
    sSpecs.surface = surface;
    sSpecs.extent = Kayou::Core::Extent2D(window->GetHeight(), window->GetWidth());
    sSpecs.imageCount = 2;
    sSpecs.presentMode = Kayou::Core::PresentMode::Mailbox;
    sSpecs.imageFormat = Kayou::Core::Format::BGRA8_SRGB;
    sSpecs.isDepthEnable = true;
    sSpecs.depthImageFormat = Kayou::Core::Format::D32_SFLOAT;

    Kayou::Core::RefCountPtr<Kayou::Core::Swapchain> swapchain = device->CreateSwapchain(sSpecs);

    Kayou::Core::BufferSpecs bufferSpecs{};
    bufferSpecs.usages = { Kayou::Core::BufferUsage::Vertex, Kayou::Core::BufferUsage::TransferDst };
    bufferSpecs.size = 65536;
    bufferSpecs.memoryAccess = { Kayou::Core::MemoryAccess::CPU_Write };

    Kayou::Core::RefCountPtr<Kayou::Core::Buffer> testBuffer = device->CreateBuffer(bufferSpecs);
    
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
