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
#include "Public/GraphicsPipeline.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4309) // Pour MSVC
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-enum-value"
#endif
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

int main()
{
#ifdef KENGINE_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    //Kayou::RHI::ShaderCompiler compiler{};
    //
    //compiler.Initialize();
    //
    //compiler.Load("Engine/Assets/Shaders/hello-world.compute.slang", Kayou::RHI::ShaderType::Compute);

    // Read base mesh 

#pragma region Mesh
    std::string meshPath = "D:/Projets/KEngine/Engine/Assets/Meshes/viking_room.obj";
    Assimp::Importer t_importer{};
    const aiScene* t_scene = t_importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);
    if (!t_scene || !t_scene->mRootNode)
    {
        spdlog::error("coudn't find path");
    }
    std::string t_directory = meshPath.substr(0, meshPath.find_last_of('/'));

    std::vector<Vertex> meshVertices;
    std::vector<int> meshIndices;
	for (unsigned int i = 0; i < t_scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = t_scene->mMeshes[i];

		uint32_t vertexOffset = static_cast<uint32_t>(meshVertices.size());

		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			Vertex vertex;

			if (mesh->HasPositions())
			{
				vertex.pos = glm::vec3(
					mesh->mVertices[v].x,
					mesh->mVertices[v].y,
					mesh->mVertices[v].z
				);
			}

			if (mesh->HasNormals())
			{
				vertex.normal = glm::vec3(
					mesh->mNormals[v].x,
					mesh->mNormals[v].y,
					mesh->mNormals[v].z
				);
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.uv = glm::vec2(
					mesh->mTextureCoords[0][v].x,
					1.f - mesh->mTextureCoords[0][v].y // Flip Y pour Vulkan
				);
			}
			else
			{
				vertex.uv = glm::vec2(0.0f, 0.0f);
			}

			meshVertices.push_back(vertex);
		}

		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{
			const aiFace& face = mesh->mFaces[f];

			for (unsigned int indexIdx = 0; indexIdx < face.mNumIndices; ++indexIdx)
			{
				meshIndices.push_back(face.mIndices[indexIdx] + vertexOffset);
			}
		}
	}
#pragma endregion

#pragma region Setup 
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
#pragma endregion

    auto copyBufferDataCommandList = device->GetCommandList(Kayou::RHI::QueueType::Graphics);
    copyBufferDataCommandList->Open();

    // Vertex Buffer 
    Kayou::RHI::BufferSpecs vertexbufferSpecs{};
    vertexbufferSpecs.primaryUsage = Kayou::RHI::BufferUsage::Vertex;
    vertexbufferSpecs.additionalUsages = { Kayou::RHI::BufferUsage::TransferDst };
    vertexbufferSpecs.size = meshVertices.size() * sizeof(Vertex);
    vertexbufferSpecs.memoryAccess = Kayou::RHI::MemoryAccess::GPU_Only;
    vertexbufferSpecs.pipelineStage = Kayou::RHI::PipelineStage::VertexInput;
    
    Kayou::Core::RefCountPtr<Kayou::RHI::Buffer> vertexBuffer = device->CreateBuffer(vertexbufferSpecs);
    copyBufferDataCommandList->SetBufferData(vertexBuffer, meshVertices.data(), meshVertices.size() * sizeof(Vertex), 0);
    
    // Index Buffer
    Kayou::RHI::BufferSpecs indexBufferSpecs{};
    indexBufferSpecs.primaryUsage = Kayou::RHI::BufferUsage::Index;
    indexBufferSpecs.additionalUsages = { Kayou::RHI::BufferUsage::TransferDst };
    indexBufferSpecs.size = meshIndices.size() * sizeof(int);
    indexBufferSpecs.memoryAccess = Kayou::RHI::MemoryAccess::GPU_Only;
    indexBufferSpecs.pipelineStage = Kayou::RHI::PipelineStage::VertexInput;
    
    Kayou::Core::RefCountPtr<Kayou::RHI::Buffer> indexBuffer = device->CreateBuffer(indexBufferSpecs);
    copyBufferDataCommandList->SetBufferData(indexBuffer, meshIndices.data(), meshIndices.size() * sizeof(int), 0);
    
    copyBufferDataCommandList->Close();
    
    Kayou::RHI::SubmitInfo submitInfo;
    submitInfo.stage = Kayou::RHI::PipelineStage::Transfer;
    
    device->SubmitCommandList(copyBufferDataCommandList, submitInfo);


    
    // Presentation images
    std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::Image>> presentationImages = device->CreatePresentationImages(swapchain);
    
    // depth images
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

    // Timeline
    Kayou::RHI::SemaphoreSpecs timelineSpecs;
    timelineSpecs.type = Kayou::RHI::SemaphoreType::Timeline;
    Kayou::Core::RefCountPtr<Kayou::RHI::Semaphore> frameTimelineSemaphore = device->CreateSemaphore(timelineSpecs);

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

    Kayou::Core::RefCountPtr<Kayou::RHI::Shader> baseVert = device->CreateShader("base.vert", Kayou::RHI::ShaderStage::Vertex, false, true);
    Kayou::Core::RefCountPtr<Kayou::RHI::Shader> unlitFrag = device->CreateShader("unlit.frag", Kayou::RHI::ShaderStage::Fragment, false, true);
    Kayou::Core::RefCountPtr<Kayou::RHI::Shader> globalLayout = device->CreateShader("globalLayout", Kayou::RHI::ShaderStage::Vertex, true, true);

    std::vector<Kayou::Core::RefCountPtr<Kayou::RHI::DescriptorSetLayout>> globalLayoutDescriptors = device->CreateDescriptorSetsLayouts(globalLayout);

    Kayou::Core::RefCountPtr<Kayou::RHI::PipelineLayout> globalPipelineLayout = device->CreatePipelineLayout(globalLayoutDescriptors, {});

    Kayou::RHI::GraphicsPipelineSpecs unlitPipelineSpecs;
    unlitPipelineSpecs.colorAttachmentCount = 1;
    unlitPipelineSpecs.colorAttachmentFormats = { swapchain->GetColorImageFormat() };
    unlitPipelineSpecs.depthAttachment = swapchain->GetDepthImageFormat();
    unlitPipelineSpecs.viewportCount = 1;
    unlitPipelineSpecs.scissorCount = 1;
    unlitPipelineSpecs.lineWidth = 1;
    unlitPipelineSpecs.cullmode = Kayou::RHI::CullMode::Back;
    unlitPipelineSpecs.frontFace = Kayou::RHI::FrontFace::CounterClockWise;
    unlitPipelineSpecs.SamplesCount = Kayou::RHI::SampleCount::Count1;
    unlitPipelineSpecs.blendColor = true;
    unlitPipelineSpecs.dynamicStates = { Kayou::RHI::DynamicState::ViewPort, Kayou::RHI::DynamicState::Scissor };
    unlitPipelineSpecs.topology = Kayou::RHI::PrimitiveTopology::TriangleList;
    unlitPipelineSpecs.shaders = { baseVert , unlitFrag };
    unlitPipelineSpecs.pipelineLayout = globalPipelineLayout;
    
    Kayou::Core::RefCountPtr<Kayou::RHI::Pipeline> unlitPipeline = device->CreateGraphicsPipeline(unlitPipelineSpecs);

    // Vertex & index buffer


    // need vertex & index buffer 
    // need texture
    // need uniform camera

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
        submitInfo.signalSemaphores = { frameTimelineSemaphore, renderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoresValues = { signalValue, 0 };
        submitInfo.stage = Kayou::RHI::PipelineStage::ColorOutput;

        device->SubmitCommandList(commandList, submitInfo);

        Kayou::RHI::PresentInfo presentInfo;
        presentInfo.waitSemaphores = { renderFinishedSemaphores[imageIndex] };
        presentInfo.swapchain = swapchain;
        presentInfo.imageIndex = imageIndex;

        device->Present(presentInfo);

        frameCounter++;
    }
    
    device->WaitIdle();
    //device->ClearQueues();
    //device->DestroyBuffer(testBuffer);

    device->DestroyBuffer(vertexBuffer);
    device->DestroyBuffer(indexBuffer);

    device->DestroyPipeline(unlitPipeline);
    device->DestroyPipelineLayout(globalPipelineLayout);
    device->DestroyDescriptorSetsLayouts(globalLayoutDescriptors);
    
    device->DestroyShader(unlitFrag);
    device->DestroyShader(baseVert);
    device->DestroyShader(globalLayout);

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

    t_importer.FreeScene();

    return 0;
}
