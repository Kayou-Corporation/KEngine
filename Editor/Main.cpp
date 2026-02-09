#include <iostream>

#include "Window/Window.hpp"
#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"
#include "Core/RHI/Public/Surface.hpp"

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
    
    while (!window->ShouldClose())
    {
        window->PollEvents();
    }
    
    instance->DestroySurface(surface);
    instance->Destroy();
    window->Destroy();

    return 0;
}