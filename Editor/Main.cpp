#include <iostream>

#include "Window/Window.hpp"
#include "Private/Vulkan/VulkanInstance.hpp"
#include "Public/Surface.hpp"
#include "Public/Device.hpp"
#include "Public/Swapchain.hpp"
#include "Public/Buffer.hpp"
#include "Public/Shader.hpp"
#include <fstream>

struct test
{
	const char* name;
	int a;
	float b;
	std::vector<float> c;
};

int main()
{
#ifdef KENGINE_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    test t;
	t.name = "test";
    t.a = 69;
	t.b = 42.0f;
	t.c = { 1.0f, 2.0f, 3.0f };
	for (int i = 0; i < 100000; ++i)
	{
		t.c.push_back(static_cast<float>(i));
	}

    std::ofstream file("Cache/Shaders/test.kayou", std::ios::binary);
    file.write((char*)&t, sizeof(t));
    file.close();

    test t2;

    std::ifstream file2("Cache/Shaders/test.kayou", std::ios::binary);
    file2.read((char*)&t2, sizeof(t2));
    file2.close();

	std::cout << t2.name << " " << t2.a << " " << t2.b << " " << t2.c[0] << " " << t2.c[1] << " " << t2.c[100002] << std::endl;

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

    Kayou::Core::RefCountPtr<Kayou::RHI::Shader> baseVert = device->CreateShader("base.vert", Kayou::RHI::ShaderStage::Vertex);
    Kayou::Core::RefCountPtr<Kayou::RHI::Shader> unlitFrag = device->CreateShader("unlit.frag", Kayou::RHI::ShaderStage::Fragment);
    
    while (!window->ShouldClose())
    {
        window->PollEvents();
    }

	device->DestroyShader(unlitFrag);
	device->DestroyShader(baseVert);
    
    device->DestroyBuffer(testBuffer);

    device->DestroySwapchain(swapchain);

    instance->DestroyDevice(device);

    instance->DestroySurface(surface);

    instance->Destroy();

    window->Destroy();

    return 0;
}
