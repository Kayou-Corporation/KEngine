#include <iostream>

#include "Window/Window.hpp"
#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"
#include "Core/RHI/Public/Surface.hpp"
#include "Core/RHI/Public/Device.hpp"

int main()
{
    RefCountPtr<Window> window = WindowInterface::InitWindow(WindowAPI::SDL);
    
    WindowSpecs specs;
    specs.width = 720;
    specs.height = 480;
    specs.name = "KEngine";
    specs.allowResize = true;
    specs.rendererAPI = RendererAPI::Vulkan;
    
    window->Create(specs);
    
    
    RefCountPtr<Instance> instance = CreateRefPtr<VulkanInstance>();
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
    dSpecs.queues = { Queue::Graphics };
    dSpecs.searchPresentQueue = true;
    dSpecs.surface = surface;

    RefCountPtr<Device> device = instance->CreateDevice(dSpecs);
    
    while (!window->ShouldClose())
    {
        window->PollEvents();
    }
    
    instance->DestroyDevice(device);
    instance->DestroySurface(surface);
    instance->Destroy();
    window->Destroy();

    return 0;
}