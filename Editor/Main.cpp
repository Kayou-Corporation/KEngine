#include <iostream>

#include "Window/Window.hpp"
#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"
#include "Core/RHI/Public/Surface.hpp"
#include "Core/RHI/Public/Device.hpp"
#include "Core/RHI/Public/Swapchain.hpp"

int main()
{
#ifdef KENGINE_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    RefCountPtr<Window> window = WindowInterface::InitWindow(WindowAPI::SDL);
    
    WindowSpecs specs;
    specs.width = 720;
    specs.height = 480;
    specs.name = "KEngine";
    specs.allowResize = true;
    specs.rendererAPI = RendererAPI::Vulkan;
    
    window->Create(specs);
    
    
    RefCountPtr<Instance> instance = RendererInterface::InitRenderer(RendererAPI::Vulkan);

    InstanceSpecs test;
    test.window = window;
    test.appVersion = Version(0, 0, 1);
    test.engineVersion = Version(0, 0, 1);
    test.debugLayers = { DebugLayers::Validation };
    
    instance->Create(test);

    RefCountPtr<Surface> surface = instance->CreateSurface({ window });

    DeviceSpecs dSpecs;
    dSpecs.gpuType = GpuType::Discrete;
    dSpecs.extensions = { Extensions::Swapchain, Extensions::DynamicRendering, Extensions::ShaderObject };
    dSpecs.queues = { QueueType::Graphics };
    dSpecs.searchPresentQueue = true;
    dSpecs.surface = surface;

    RefCountPtr<Device> device = instance->CreateDevice(dSpecs);

    SwapchainSpecs sSpecs;
    sSpecs.surface = surface;
    sSpecs.extent = Extent2D(window->GetHeight(), window->GetWidth());
    sSpecs.imageCount = 2;
    sSpecs.presentMode = PresentMode::Mailbox;
    sSpecs.imageFormat = Format::BGRA8_SRGB;
    sSpecs.isDepthEnable = true;
    sSpecs.depthImageFormat = Format::D32_SFLOAT;

    RefCountPtr<Swapchain> swapchain = device->CreateSwapchain(sSpecs);
    
    while (!window->ShouldClose())
    {
        window->PollEvents();
    }
    
    device->DestroySwapchain(swapchain);

    instance->DestroyDevice(device);

    instance->DestroySurface(surface);

    instance->Destroy();

    window->Destroy();

    return 0;
}