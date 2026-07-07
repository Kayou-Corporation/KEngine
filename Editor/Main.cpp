#include <iostream>

#include "Window.hpp"
#include "WindowRenderer.hpp"
#include "RHI/API/Surface.hpp"
#include "RHI/API/Instance.hpp"
#include "RHI/API/Device.hpp"
#include "RHI/API/Swapchain.hpp"
#include "RHI/API/Buffer.hpp"
#include "RHI/API/Shader.hpp"
#include "RHI/API/Image.hpp"
#include "RHI/API/Renderpass.hpp"
#include "RHI/API/CommandList.hpp"
#include "RHI/API/Syncronisation.hpp"
#include "RHI/API/GraphicsPipeline.hpp"
#include "RHI/API/Sampler.hpp"
#include "RHI/API/DescriptorSet.hpp"
#include "RHI/API/RHI.hpp"
#include "Camera/EditorCamera.hpp"

USING_KAYOU

DISABLE_WARNINGS

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4309)
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

RESTORE_WARNINGS


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct CameraData
{
    glm::mat4 cameraVP;
    glm::vec3 cameraPos;
};

struct Model
{
    glm::mat4 model;
    glm::mat4 normal;
};

int main()
{
#ifdef KENGINE_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

#pragma region Mesh
    std::string meshPath = "Engine/Assets/Meshes/viking_room.obj";
    Assimp::Importer t_importer{};
    const aiScene* t_scene = t_importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);
    if (!t_scene || !t_scene->mRootNode)
    {
        spdlog::error("coudn't find path");
    }
    std::string t_directory = meshPath.substr(0, meshPath.find_last_of('/'));

    std::vector<Vertex> meshVertices;
    std::vector<uint32_t> meshIndices;
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

#pragma region Texture

    std::string texturePath = "Engine/Assets/Textures/viking_room.png";
    int t_texWidth, t_texHeight, t_texChannels;
    stbi_uc* texturePixels = stbi_load(texturePath.c_str(), &t_texWidth, &t_texHeight, &t_texChannels, STBI_rgb_alpha);
   
#pragma endregion

#pragma region Setup 
    Core::RefCountPtr<Window::Window> window = Window::WindowInterface::InitWindow(Window::WindowAPI::SDL);
 
    Window::WindowSpecs specs;
    specs.width = 720;
    specs.height = 480;
    specs.name = "KEngine";
    specs.allowResize = true;
    specs.rendererAPI = Core::RendererAPI::Vulkan;
    
    window->Create(specs);

    float aspectRatio = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());

    Core::EditorCamera editorCamera;
    editorCamera.SetPosition(glm::vec3(0, 0, 5.0f));
    editorCamera.SetRotation(glm::vec3(0, 0, 0.f));
    editorCamera.SetFov(glm::radians(45.f));
    editorCamera.SetAspectRatio(aspectRatio);
    editorCamera.SetNearPlane(0.1f);
    editorCamera.SetFarPlane(100.0f);

    editorCamera.RecalculateMatrices();

    //glm::vec3 cameraPos = glm::vec3(0, 0, 5.0f);
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    //glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);
    //
    //float fov = glm::radians(45.f);
    //float aspectRatio = static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight());
    //float nearPlane = 0.1f;
    //float farPlane = 100.0f;
    //glm::mat4 proj = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    //proj[1][1] *= -1;

    CameraData initCam;
    initCam.cameraVP = editorCamera.GetViewProjectionMatrix();
    initCam.cameraPos = editorCamera.GetPosition();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.0f, 0, 0.0f));
    model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    Model mdl;
    mdl.model = glm::transpose(model);
    mdl.normal = normalMatrix;

    RHI::InstanceHandle instance = RHI::RendererInterface::InitRenderer(Core::RendererAPI::Vulkan);

    RHI::InstanceSpecs test;
    test.window = window;
    test.appVersion = RHI::Version(0, 0, 1);
    test.engineVersion = RHI::Version(0, 0, 1);
    test.debugLayers = {RHI::DebugLayers::Validation };
    
    instance->Create(test);

    RHI::SurfaceHandle surface = instance->CreateSurface({ window });

    RHI::DeviceSpecs dSpecs;
    dSpecs.gpuType = RHI::GpuType::Discrete;
    dSpecs.extensions = {RHI::Extensions::Swapchain, RHI::Extensions::DynamicRendering, RHI::Extensions::ShaderObject };
    dSpecs.queues = {RHI::QueueType::Graphics };
    dSpecs.searchPresentQueue = true;
    dSpecs.surface = surface;

    RHI::DeviceHandle device = instance->CreateDevice(dSpecs);

    RHI::SwapchainSpecs sSpecs;
    sSpecs.surface = surface;
    sSpecs.extent = RHI::Extent2D(window->GetWidth(), window->GetHeight());
    sSpecs.imageCount = 2;
    sSpecs.presentMode = RHI::PresentMode::Mailbox;
    sSpecs.imageFormat = RHI::Format::BGRA8_SRGB;
    sSpecs.isDepthEnable = true;
    sSpecs.depthImageFormat = RHI::Format::D32_SFLOAT;
    RHI::SwapchainHandle swapchain = device->CreateSwapchain(sSpecs);
#pragma endregion

    // Vertex Buffer 
    RHI::BufferSpecs vertexbufferSpecs{};
    vertexbufferSpecs.primaryUsage = RHI::BufferUsage::Vertex;
    vertexbufferSpecs.additionalUsages = { RHI::BufferUsage::TransferDst };
    vertexbufferSpecs.size = meshVertices.size() * sizeof(Vertex);
    vertexbufferSpecs.memoryAccess = RHI::MemoryAccess::GpuOnly;
    vertexbufferSpecs.pipelineStage = RHI::PipelineStage::VertexInput;
    RHI::BufferHandle vertexBuffer = device->CreateBuffer(vertexbufferSpecs);
    
    // Index Buffer
    RHI::BufferSpecs indexBufferSpecs{};
    indexBufferSpecs.primaryUsage = RHI::BufferUsage::Index;
    indexBufferSpecs.additionalUsages = { RHI::BufferUsage::TransferDst };
    indexBufferSpecs.size = meshIndices.size() * sizeof(uint32_t);
    indexBufferSpecs.memoryAccess = RHI::MemoryAccess::GpuOnly;
    indexBufferSpecs.pipelineStage = RHI::PipelineStage::VertexInput;
    RHI::BufferHandle indexBuffer = device->CreateBuffer(indexBufferSpecs);

    // Base Texture
    RHI::ImageSpecs textureImageSpecs;
    textureImageSpecs.format = RHI::Format::RGBA8_SRGB;
    textureImageSpecs.targetLayout = RHI::Layout::ShaderReadOnly;
    textureImageSpecs.type = RHI::ImageType::Image2D;
    textureImageSpecs.viewType = RHI::ImageViewType::Image2D;
    textureImageSpecs.viewAspect = RHI::ImageViewAspect::Color;
    textureImageSpecs.usages = { RHI::ImageUsage::TransferDst, RHI::ImageUsage::ShaderSampled };
    textureImageSpecs.extent = { static_cast<uint32_t>(t_texWidth), static_cast<uint32_t>(t_texHeight), 1 };
    RHI::ImageHandle textureImage = device->CreateImage(textureImageSpecs);

    // Base Sampler
    RHI::SamplerSpecs samplerSpecs;
    samplerSpecs.magFilter = RHI::Filter::Linear;
    samplerSpecs.minFilter = RHI::Filter::Linear;
    samplerSpecs.mipmapMode = RHI::SamplerMipmapMode::Linear;
    samplerSpecs.addressU = RHI::SamplerAddressMode::Repeat;
    samplerSpecs.addressV = RHI::SamplerAddressMode::Repeat;
    samplerSpecs.addressW = RHI::SamplerAddressMode::Repeat;
    samplerSpecs.mipLodBias = 0.0f;
    samplerSpecs.anisotropyEnable = false;
    samplerSpecs.maxAnisotropy = 1.0f;
    samplerSpecs.compareEnable = false;
    samplerSpecs.compareOp = RHI::CompareOp::Always;
    samplerSpecs.minLod = 0.0f;
    samplerSpecs.maxLod = 0.0f;
    samplerSpecs.borderColor = RHI::BorderColor::IntOpaqueBlack;
    samplerSpecs.unnormalizedCoordinates = false;

    RHI::SamplerHandle sampler = device->CreateSampler(samplerSpecs);


    RHI::CommandListHandle copyBufferDataCommandList = device->GetCommandList(RHI::QueueType::Graphics);

    copyBufferDataCommandList->Open();
    
    copyBufferDataCommandList->SetBufferData(vertexBuffer, meshVertices.data(), meshVertices.size() * sizeof(Vertex), 0);
    
    copyBufferDataCommandList->SetBufferData(indexBuffer, meshIndices.data(), meshIndices.size() * sizeof(uint32_t), 0);
    
    copyBufferDataCommandList->SetImageData(textureImage, texturePixels, t_texWidth * t_texHeight * 4);

    copyBufferDataCommandList->TransitionImageLayout(textureImage,RHI::Layout::ShaderReadOnly);

    copyBufferDataCommandList->Close();
    
    RHI::SubmitInfo submitInfocopyBufferData;
    submitInfocopyBufferData.stage = RHI::PipelineStage::Transfer;
    
    device->SubmitCommandList(copyBufferDataCommandList, submitInfocopyBufferData);

    // Uniform buffer camera
    RHI::BufferSpecs uniformCameraSpecs{};
    uniformCameraSpecs.primaryUsage = RHI::BufferUsage::Uniform;
    uniformCameraSpecs.size = sizeof(CameraData);
    uniformCameraSpecs.memoryAccess = RHI::MemoryAccess::Upload;
    uniformCameraSpecs.pipelineStage = RHI::PipelineStage::VertexShader;
    uniformCameraSpecs.isPersistentMapped = true;
    uniformCameraSpecs.additionalUsages = { RHI::BufferUsage::TransferDst };
    RHI::BufferHandle uniformCamera = device->CreateBuffer(uniformCameraSpecs);


    // Uniform buffer modelMatrix
    RHI::BufferSpecs uniformModelSpecs{};
    uniformModelSpecs.primaryUsage = RHI::BufferUsage::Uniform;
    uniformModelSpecs.size = sizeof(Model);
    uniformModelSpecs.memoryAccess = RHI::MemoryAccess::Upload;
    uniformModelSpecs.pipelineStage = RHI::PipelineStage::VertexShader;
    uniformModelSpecs.isPersistentMapped = true;
    uniformModelSpecs.additionalUsages = { RHI::BufferUsage::TransferDst };
    RHI::BufferHandle uniformModel = device->CreateBuffer(uniformModelSpecs);

    auto copyUniformDataCommandList = device->GetCommandList(RHI::QueueType::Graphics);
    copyUniformDataCommandList->Open();

    copyUniformDataCommandList->SetBufferData(uniformCamera, &initCam, sizeof(CameraData), 0);
    copyUniformDataCommandList->SetBufferData(uniformModel, &mdl, sizeof(Model), 0);

    copyUniformDataCommandList->Close();

    RHI::SubmitInfo submitInfocopyUniformData;
    submitInfocopyUniformData.stage = RHI::PipelineStage::Transfer;

    device->SubmitCommandList(copyUniformDataCommandList, submitInfocopyUniformData);

    // Presentation images
    std::vector<RHI::ImageHandle> presentationImages = device->CreatePresentationImages(swapchain);
    
    // depth images
    RHI::SwapchainImageSpecs depthImageSpecs; 
    depthImageSpecs.imageType = RHI::SwapchainImageType::Depth;
    depthImageSpecs.targetLayout = RHI::Layout::DepthStencilAttachment;
    depthImageSpecs.type = RHI::ImageType::Image2D;
    depthImageSpecs.usages = { RHI::ImageUsage::DepthStencilAttachment };
    depthImageSpecs.viewType = RHI::ImageViewType::Image2D;
    depthImageSpecs.viewAspect = RHI::ImageViewAspect::Depth;

    RHI::ImageHandle depthImage = device->CreateImagesWithSwapchain(depthImageSpecs, swapchain);

    // Timeline
    RHI::SemaphoreSpecs timelineSpecs;
    timelineSpecs.type = RHI::SemaphoreType::Timeline;
    RHI::SemaphoreHandle frameTimelineSemaphore = device->CreateSemaphore(timelineSpecs);

    // Binary 
    std::vector<RHI::SemaphoreHandle> imageAvailablesSemaphores;
    std::vector<RHI::SemaphoreHandle> renderFinishedSemaphores;

    for (uint32_t i = 0; i < swapchain->GetImageCount(); ++i) 
    {
        RHI::SemaphoreSpecs binarySpecs{};
        binarySpecs.type = RHI::SemaphoreType::Binary;

        RHI::SemaphoreHandle imageAvailableSemaphore = device->CreateSemaphore(binarySpecs);
        RHI::SemaphoreHandle renderFinishedSemaphore = device->CreateSemaphore(binarySpecs);

        imageAvailablesSemaphores.push_back(imageAvailableSemaphore);
        renderFinishedSemaphores.push_back(renderFinishedSemaphore);
    }

    RHI::ShaderHandle baseVert = device->CreateShader("base.vert", RHI::ShaderStage::Vertex, false, true);
    RHI::ShaderHandle unlitFrag = device->CreateShader("unlit.frag", RHI::ShaderStage::Fragment, false, true);
    RHI::ShaderHandle globalLayoutShader = device->CreateShader("globalLayout", RHI::ShaderStage::Vertex, true, true);

    std::vector<RHI::DescriptorSetLayoutHandle> globalLayoutDescriptors = device->CreateDescriptorSetsLayouts(globalLayoutShader);

    RHI::PipelineLayoutHandle globalPipelineLayout = device->CreatePipelineLayout(globalLayoutDescriptors, {});

    RHI::GraphicsPipelineSpecs unlitPipelineSpecs;
    unlitPipelineSpecs.colorAttachmentCount = 1;
    unlitPipelineSpecs.colorAttachmentFormats = { swapchain->GetColorImageFormat() };
    unlitPipelineSpecs.depthAttachment = swapchain->GetDepthImageFormat();
    unlitPipelineSpecs.viewportCount = 1;
    unlitPipelineSpecs.scissorCount = 1;
    unlitPipelineSpecs.lineWidth = 1;
    unlitPipelineSpecs.cullmode = RHI::CullMode::Back;
    unlitPipelineSpecs.frontFace = RHI::FrontFace::CounterClockWise;
    unlitPipelineSpecs.SamplesCount = RHI::SampleCount::Count1;
    unlitPipelineSpecs.blendColor = false;
    unlitPipelineSpecs.depthTest = true;
    unlitPipelineSpecs.depthWrite = true;
    unlitPipelineSpecs.depthCompare = RHI::CompareOp::LessOrEqual;
    unlitPipelineSpecs.dynamicStates = { RHI::DynamicState::ViewPort, RHI::DynamicState::Scissor };
    unlitPipelineSpecs.topology = RHI::PrimitiveTopology::TriangleList;
    unlitPipelineSpecs.shaders = { baseVert , unlitFrag };
    unlitPipelineSpecs.pipelineLayout = globalPipelineLayout;
    
    RHI::PipelineHandle unlitPipeline = device->CreateGraphicsPipeline(unlitPipelineSpecs);
    
    // DescriptorSet
    RHI::DescriptorSetLayoutHandle frameDataLayout = globalPipelineLayout->GetDescriptorSetLayout("frameData");
    RHI::DescriptorSetLayoutHandle drawDataLayout = globalPipelineLayout->GetDescriptorSetLayout("drawData");
    
    RHI::DescriptorSetHandle frameDataDescriptorSet = device->CreateDescriptorSet(frameDataLayout);
    RHI::DescriptorSetHandle drawDataDescriptorSet = device->CreateDescriptorSet(drawDataLayout);

    device->SetDescriptorSetBuffer(frameDataDescriptorSet, "camera", RHI::DescriptorType::UniformBuffer, uniformCamera, 0, sizeof(CameraData));
    device->SetDescriptorSetBuffer(drawDataDescriptorSet, "object", RHI::DescriptorType::UniformBuffer, uniformModel, 0, sizeof(Model));
    device->SetDescriptorSetImage(drawDataDescriptorSet, "texture2D", RHI::DescriptorType::SampledImage, textureImage, nullptr);
    device->SetDescriptorSetSampler(drawDataDescriptorSet, "sampler", RHI::DescriptorType::Sampler, sampler);
    
    device->WaitIdle();

    uint64_t frameCounter = 0;
    bool gpuResizeRequest = false;
    while (!window->ShouldClose())
    {
        window->PollEvents();
         
        if (window->GetHasResize() || gpuResizeRequest)
        {
            device->WaitIdle();
    
            // Destroy everything.
            device->DestroyPresentationImages(presentationImages);
            device->DestroyImage(depthImage);
            device->DestroySwapchain(swapchain);
    
            // Update everything
            device->UpdateCompatibility(surface);
    
            // Recreate everything.
            uint32_t tWidth = window->GetWidth();
            uint32_t tHeight = window->GetHeight();
            sSpecs.extent = RHI::Extent2D(tWidth, tHeight);
            swapchain = device->CreateSwapchain(sSpecs);
            presentationImages = device->CreatePresentationImages(swapchain);
            depthImage = device->CreateImagesWithSwapchain(depthImageSpecs, swapchain);
    
            window->ResizeComplete();
            gpuResizeRequest = false;
    
            editorCamera.SetAspectRatio(static_cast<float>(tWidth) / static_cast<float>(tHeight));
            editorCamera.RecalculateMatrices();
    
            CameraData resizeCam;
            resizeCam.cameraVP = editorCamera.GetViewProjectionMatrix();
            resizeCam.cameraPos = editorCamera.GetPosition();
            
            auto commandList = device->GetCommandList(RHI::QueueType::Graphics);
            commandList->Open();
            commandList->SetBufferData(uniformCamera, &resizeCam, sizeof(CameraData), 0);
            commandList->Close();
            
            RHI::SubmitInfo submitTransfer;
            RHI::SubmitInfo submitInfo;
            submitInfo.stage = RHI::PipelineStage::None;
            
            device->SubmitCommandList(commandList, submitTransfer);
            //device->SetDescriptorSetBuffer(frameDataDescriptorSet, "camera", RHI::DescriptorType::UniformBuffer, uniformCamera, 0, sizeof(Camera));
        }
    
        uint32_t maxFramesInFlight = swapchain->GetImageCount();
        uint32_t syncIndex = frameCounter % maxFramesInFlight;
    
        if (frameCounter >= maxFramesInFlight)
        {
            uint64_t waitValue = frameCounter - maxFramesInFlight + 1;
            device->WaitForSemaphore(frameTimelineSemaphore, waitValue);
        }
    
        uint32_t imageIndex = device->AcquirreNextImage(swapchain, imageAvailablesSemaphores[syncIndex]);
    
        device->RunGarbageCollector();
    
        RHI::RenderingAttachment colorAttachment;
        colorAttachment.image = presentationImages[imageIndex];
        colorAttachment.layout = RHI::Layout::ColorAttachment;
        colorAttachment.loadOp = RHI::LoadOp::Clear;
        colorAttachment.storeOp = RHI::StoreOp::Store;
        colorAttachment.clearValueColor = RHI::ClearValue(0.1f, 0.1f, 0.1f, 1.0f);
    
        RHI::RenderingAttachment depthAttachment;
        depthAttachment.image = depthImage;
        depthAttachment.layout = RHI::Layout::DepthStencilAttachment;
        depthAttachment.loadOp = RHI::LoadOp::Clear;
        depthAttachment.storeOp = RHI::StoreOp::Store;
        depthAttachment.clearValueDepth = RHI::ClearValue(1.0f, 0.f, 0.f, 0.f);
    
        RHI::RenderingInfo renderingInfo;
        renderingInfo.offset = { 0, 0 };
        renderingInfo.extent = { window->GetWidth(), window->GetHeight()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.colorAttachments = { colorAttachment };
        renderingInfo.depthAttachment = depthAttachment;
    
        auto commandList = device->GetCommandList(RHI::QueueType::Graphics);
        commandList->Open();
    
        commandList->TransitionImageLayout(presentationImages[imageIndex], RHI::Layout::ColorAttachment);
        commandList->TransitionImageLayout(depthImage, RHI::Layout::DepthStencilAttachment);
    
        commandList->BeginRendering(renderingInfo);
    
        commandList->SetViewport(0, 0, window->GetWidth(), window->GetHeight());
        commandList->SetScissor(0, 0, window->GetWidth(), window->GetHeight());
    
        commandList->BindPipeline(unlitPipeline);
    
        commandList->BindDescriptorSet(globalPipelineLayout, "frameData", frameDataDescriptorSet, RHI::PipelineBindPoint::Graphics);
        commandList->BindDescriptorSet(globalPipelineLayout, "drawData", drawDataDescriptorSet, RHI::PipelineBindPoint::Graphics);
    
        commandList->BindVertexBuffer(vertexBuffer, 0);
        commandList->BindIndexBuffer(indexBuffer, 0);
        
        commandList->DrawIndexed(meshIndices.size(), 1, 0, 0, 0);
    
        commandList->EndRendering();
    
        commandList->TransitionImageLayout(presentationImages[imageIndex], RHI::Layout::Present);
    
        commandList->Close();
    
        uint64_t signalValue = frameCounter + 1;
    
        RHI::SubmitInfo submitInfo;
        submitInfo.waitSemaphores = { imageAvailablesSemaphores[syncIndex] };
        submitInfo.waitSemaphoresValues = { 0 };
        submitInfo.signalSemaphores = { frameTimelineSemaphore, renderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoresValues = { signalValue, 0 };
        submitInfo.stage = RHI::PipelineStage::ColorOutput;
    
        device->SubmitCommandList(commandList, submitInfo);
    
        RHI::PresentInfo presentInfo;
        presentInfo.waitSemaphores = { renderFinishedSemaphores[imageIndex] };
        presentInfo.swapchain = swapchain;
        presentInfo.imageIndex = imageIndex;
    
        bool sucess = device->Present(presentInfo);
    
        if (!sucess)
        {
            gpuResizeRequest = true;
        }
    
        frameCounter++;
    }
    
    device->WaitIdle();

    device->DestroyBuffer(vertexBuffer);
    device->DestroyBuffer(indexBuffer);
    device->DestroyImage(textureImage);
    device->DestroySampler(sampler);
    device->DestroyBuffer(uniformCamera);
    device->DestroyBuffer(uniformModel);

    device->DestroyDescriptorSet(frameDataDescriptorSet);
    device->DestroyDescriptorSet(drawDataDescriptorSet);

    device->DestroyPipeline(unlitPipeline);
    device->DestroyPipelineLayout(globalPipelineLayout);
    device->DestroyDescriptorSetsLayouts(globalLayoutDescriptors);
    
    device->DestroyShader(unlitFrag);
    device->DestroyShader(baseVert);
    device->DestroyShader(globalLayoutShader);

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
    stbi_image_free(texturePixels);

    return 0;
}
