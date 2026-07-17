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

#include "BuildMode.hpp"

USING_KAYOU

DISABLE_WARNINGS

#ifdef KCOMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4309)
#endif

#ifdef KCOMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-enum-value"
#endif
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#ifdef KCOMPILER_CLANG
#pragma clang diagnostic pop
#endif

#ifdef KCOMPILER_MSVC
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
#ifdef KDEBUG
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

    Core::InstanceHandle instance = Core::RendererInterface::InitRenderer(Core::RendererAPI::Vulkan);

    Core::InstanceSpecs test;
    test.window = window;
    test.appVersion = Core::Version(0, 0, 1);
    test.engineVersion = Core::Version(0, 0, 1);
    test.debugLayers = {Core::DebugLayers::Validation };
    
    instance->Create(test);

    Core::SurfaceHandle surface = instance->CreateSurface({ window });

    Core::DeviceSpecs dSpecs;
    dSpecs.gpuType = Core::GpuType::Discrete;
    dSpecs.extensions = {Core::Extensions::Swapchain, Core::Extensions::DynamicRendering, Core::Extensions::ShaderObject };
    dSpecs.queues = {Core::QueueType::Graphics };
    dSpecs.searchPresentQueue = true;
    dSpecs.surface = surface;

    Core::DeviceHandle device = instance->CreateDevice(dSpecs);

    Core::SwapchainSpecs sSpecs;
    sSpecs.surface = surface;
    sSpecs.extent = Core::Extent2D(window->GetWidth(), window->GetHeight());
    sSpecs.imageCount = 2;
    sSpecs.presentMode = Core::PresentMode::Mailbox;
    sSpecs.imageFormat = Core::Format::BGRA8_SRGB;
    sSpecs.isDepthEnable = true;
    sSpecs.depthImageFormat = Core::Format::D32_SFLOAT;
    Core::SwapchainHandle swapchain = device->CreateSwapchain(sSpecs);
#pragma endregion

    // Vertex Buffer 
    Core::BufferSpecs vertexbufferSpecs{};
    vertexbufferSpecs.primaryUsage = Core::BufferUsage::Vertex;
    vertexbufferSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    vertexbufferSpecs.size = static_cast<uint32_t>(meshVertices.size()) * sizeof(Vertex);
    vertexbufferSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    vertexbufferSpecs.pipelineStage = Core::PipelineStage::VertexInput;
    Core::BufferHandle vertexBuffer = device->CreateBuffer(vertexbufferSpecs);
    
    // Index Buffer
    Core::BufferSpecs indexBufferSpecs{};
    indexBufferSpecs.primaryUsage = Core::BufferUsage::Index;
    indexBufferSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    indexBufferSpecs.size = static_cast<uint32_t>(meshIndices.size()) * sizeof(uint32_t);
    indexBufferSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    indexBufferSpecs.pipelineStage = Core::PipelineStage::VertexInput;
    Core::BufferHandle indexBuffer = device->CreateBuffer(indexBufferSpecs);

    // Base Texture
    Core::ImageSpecs textureImageSpecs;
    textureImageSpecs.format = Core::Format::RGBA8_SRGB;
    textureImageSpecs.targetLayout = Core::Layout::ShaderReadOnly;
    textureImageSpecs.type = Core::ImageType::Image2D;
    textureImageSpecs.viewType = Core::ImageViewType::Image2D;
    textureImageSpecs.viewAspect = Core::ImageViewAspect::Color;
    textureImageSpecs.usages = { Core::ImageUsage::TransferDst, Core::ImageUsage::ShaderSampled };
    textureImageSpecs.extent = { static_cast<uint32_t>(t_texWidth), static_cast<uint32_t>(t_texHeight), 1 };
    Core::ImageHandle textureImage = device->CreateImage(textureImageSpecs);

    // Base Sampler
    Core::SamplerSpecs samplerSpecs;
    samplerSpecs.magFilter = Core::Filter::Linear;
    samplerSpecs.minFilter = Core::Filter::Linear;
    samplerSpecs.mipmapMode = Core::SamplerMipmapMode::Linear;
    samplerSpecs.addressU = Core::SamplerAddressMode::Repeat;
    samplerSpecs.addressV = Core::SamplerAddressMode::Repeat;
    samplerSpecs.addressW = Core::SamplerAddressMode::Repeat;
    samplerSpecs.mipLodBias = 0.0f;
    samplerSpecs.anisotropyEnable = false;
    samplerSpecs.maxAnisotropy = 1.0f;
    samplerSpecs.compareEnable = false;
    samplerSpecs.compareOp = Core::CompareOp::Always;
    samplerSpecs.minLod = 0.0f;
    samplerSpecs.maxLod = 0.0f;
    samplerSpecs.borderColor = Core::BorderColor::IntOpaqueBlack;
    samplerSpecs.unnormalizedCoordinates = false;

    Core::SamplerHandle sampler = device->CreateSampler(samplerSpecs);


    Core::CommandListHandle copyBufferDataCommandList = device->GetCommandList(Core::QueueType::Graphics);

    copyBufferDataCommandList->Open();
    
    copyBufferDataCommandList->SetBufferData(vertexBuffer, meshVertices.data(), static_cast<uint32_t>(meshVertices.size()) * sizeof(Vertex), 0);
    
    copyBufferDataCommandList->SetBufferData(indexBuffer, meshIndices.data(), static_cast<uint32_t>(meshIndices.size()) * sizeof(uint32_t), 0);
    
    copyBufferDataCommandList->SetImageData(textureImage, texturePixels, t_texWidth * t_texHeight * 4);

    copyBufferDataCommandList->TransitionImageLayout(textureImage,Core::Layout::ShaderReadOnly);

    copyBufferDataCommandList->Close();
    
    Core::SubmitInfo submitInfocopyBufferData;
    submitInfocopyBufferData.stage = Core::PipelineStage::Transfer;
    
    device->SubmitCommandList(copyBufferDataCommandList, submitInfocopyBufferData);

    // Uniform buffer camera
    Core::BufferSpecs uniformCameraSpecs{};
    uniformCameraSpecs.primaryUsage = Core::BufferUsage::Uniform;
    uniformCameraSpecs.size = sizeof(CameraData);
    uniformCameraSpecs.memoryAccess = Core::MemoryAccess::Upload;
    uniformCameraSpecs.pipelineStage = Core::PipelineStage::VertexShader;
    uniformCameraSpecs.isPersistentMapped = true;
    uniformCameraSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    Core::BufferHandle uniformCamera = device->CreateBuffer(uniformCameraSpecs);


    // Uniform buffer modelMatrix
    Core::BufferSpecs uniformModelSpecs{};
    uniformModelSpecs.primaryUsage = Core::BufferUsage::Uniform;
    uniformModelSpecs.size = sizeof(Model);
    uniformModelSpecs.memoryAccess = Core::MemoryAccess::Upload;
    uniformModelSpecs.pipelineStage = Core::PipelineStage::VertexShader;
    uniformModelSpecs.isPersistentMapped = true;
    uniformModelSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    Core::BufferHandle uniformModel = device->CreateBuffer(uniformModelSpecs);

    auto copyUniformDataCommandList = device->GetCommandList(Core::QueueType::Graphics);
    copyUniformDataCommandList->Open();

    copyUniformDataCommandList->SetBufferData(uniformCamera, &initCam, sizeof(CameraData), 0);
    copyUniformDataCommandList->SetBufferData(uniformModel, &mdl, sizeof(Model), 0);

    copyUniformDataCommandList->Close();

    Core::SubmitInfo submitInfocopyUniformData;
    submitInfocopyUniformData.stage = Core::PipelineStage::Transfer;

    device->SubmitCommandList(copyUniformDataCommandList, submitInfocopyUniformData);

    // Presentation images
    std::vector<Core::ImageHandle> presentationImages = device->CreatePresentationImages(swapchain);
    
    // depth images
    Core::SwapchainImageSpecs depthImageSpecs; 
    depthImageSpecs.imageType = Core::SwapchainImageType::Depth;
    depthImageSpecs.targetLayout = Core::Layout::DepthStencilAttachment;
    depthImageSpecs.type = Core::ImageType::Image2D;
    depthImageSpecs.usages = { Core::ImageUsage::DepthStencilAttachment };
    depthImageSpecs.viewType = Core::ImageViewType::Image2D;
    depthImageSpecs.viewAspect = Core::ImageViewAspect::Depth;

    Core::ImageHandle depthImage = device->CreateImagesWithSwapchain(depthImageSpecs, swapchain);

    // Timeline
    Core::SemaphoreSpecs timelineSpecs;
    timelineSpecs.type = Core::SemaphoreType::Timeline;
    Core::SemaphoreHandle frameTimelineSemaphore = device->CreateSemaphore(timelineSpecs);

    // Binary 
    std::vector<Core::SemaphoreHandle> imageAvailablesSemaphores;
    std::vector<Core::SemaphoreHandle> renderFinishedSemaphores;

    for (uint32_t i = 0; i < swapchain->GetImageCount(); ++i) 
    {
        Core::SemaphoreSpecs binarySpecs{};
        binarySpecs.type = Core::SemaphoreType::Binary;

        Core::SemaphoreHandle imageAvailableSemaphore = device->CreateSemaphore(binarySpecs);
        Core::SemaphoreHandle renderFinishedSemaphore = device->CreateSemaphore(binarySpecs);

        imageAvailablesSemaphores.push_back(imageAvailableSemaphore);
        renderFinishedSemaphores.push_back(renderFinishedSemaphore);
    }

    Core::ShaderHandle baseVert = device->CreateShader("base.vert", Core::ShaderStage::Vertex, false, true);
    Core::ShaderHandle unlitFrag = device->CreateShader("unlit.frag", Core::ShaderStage::Fragment, false, true);
    Core::ShaderHandle globalLayoutShader = device->CreateShader("globalLayout", Core::ShaderStage::Vertex, true, true);

    std::vector<Core::DescriptorSetLayoutHandle> globalLayoutDescriptors = device->CreateDescriptorSetsLayouts(globalLayoutShader);

    Core::PipelineLayoutHandle globalPipelineLayout = device->CreatePipelineLayout(globalLayoutDescriptors, {});

    Core::GraphicsPipelineSpecs unlitPipelineSpecs;
    unlitPipelineSpecs.colorAttachmentCount = 1;
    unlitPipelineSpecs.colorAttachmentFormats = { swapchain->GetColorImageFormat() };
    unlitPipelineSpecs.depthAttachment = swapchain->GetDepthImageFormat();
    unlitPipelineSpecs.viewportCount = 1;
    unlitPipelineSpecs.scissorCount = 1;
    unlitPipelineSpecs.lineWidth = 1.f;
    unlitPipelineSpecs.cullmode = Core::CullMode::Back;
    unlitPipelineSpecs.frontFace = Core::FrontFace::CounterClockWise;
    unlitPipelineSpecs.SamplesCount = Core::SampleCount::Count1;
    unlitPipelineSpecs.blendColor = false;
    unlitPipelineSpecs.depthTest = true;
    unlitPipelineSpecs.depthWrite = true;
    unlitPipelineSpecs.depthCompare = Core::CompareOp::LessOrEqual;
    unlitPipelineSpecs.dynamicStates = { Core::DynamicState::ViewPort, Core::DynamicState::Scissor };
    unlitPipelineSpecs.topology = Core::PrimitiveTopology::TriangleList;
    unlitPipelineSpecs.shaders = { baseVert , unlitFrag };
    unlitPipelineSpecs.pipelineLayout = globalPipelineLayout;
    
    Core::PipelineHandle unlitPipeline = device->CreateGraphicsPipeline(unlitPipelineSpecs);
    
    // DescriptorSet
    Core::DescriptorSetLayoutHandle frameDataLayout = globalPipelineLayout->GetDescriptorSetLayout("frameData");
    Core::DescriptorSetLayoutHandle drawDataLayout = globalPipelineLayout->GetDescriptorSetLayout("drawData");
    
    Core::DescriptorSetHandle frameDataDescriptorSet = device->CreateDescriptorSet(frameDataLayout);
    Core::DescriptorSetHandle drawDataDescriptorSet = device->CreateDescriptorSet(drawDataLayout);

    device->SetDescriptorSetBuffer(frameDataDescriptorSet, "camera", Core::DescriptorType::UniformBuffer, uniformCamera, 0, sizeof(CameraData));
    device->SetDescriptorSetBuffer(drawDataDescriptorSet, "object", Core::DescriptorType::UniformBuffer, uniformModel, 0, sizeof(Model));
    device->SetDescriptorSetImage(drawDataDescriptorSet, "texture2D", Core::DescriptorType::SampledImage, textureImage, nullptr);
    device->SetDescriptorSetSampler(drawDataDescriptorSet, "sampler", Core::DescriptorType::Sampler, sampler);
    
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
            sSpecs.extent = Core::Extent2D(tWidth, tHeight);
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
            
            auto commandList = device->GetCommandList(Core::QueueType::Graphics);
            commandList->Open();
            commandList->SetBufferData(uniformCamera, &resizeCam, sizeof(CameraData), 0);
            commandList->Close();
            
            Core::SubmitInfo submitTransfer;
            Core::SubmitInfo submitInfo;
            submitInfo.stage = Core::PipelineStage::None;
            
            device->SubmitCommandList(commandList, submitTransfer);
            //device->SetDescriptorSetBuffer(frameDataDescriptorSet, "camera", Core::DescriptorType::UniformBuffer, uniformCamera, 0, sizeof(Camera));
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
    
        Core::RenderingAttachment colorAttachment;
        colorAttachment.image = presentationImages[imageIndex];
        colorAttachment.layout = Core::Layout::ColorAttachment;
        colorAttachment.loadOp = Core::LoadOp::Clear;
        colorAttachment.storeOp = Core::StoreOp::Store;
        colorAttachment.clearValueColor = Core::ClearValue(0.1f, 0.1f, 0.1f, 1.0f);
    
        Core::RenderingAttachment depthAttachment;
        depthAttachment.image = depthImage;
        depthAttachment.layout = Core::Layout::DepthStencilAttachment;
        depthAttachment.loadOp = Core::LoadOp::Clear;
        depthAttachment.storeOp = Core::StoreOp::Store;
        depthAttachment.clearValueDepth = Core::ClearValue(1.0f, 0.f, 0.f, 0.f);
    
        Core::RenderingInfo renderingInfo;
        renderingInfo.offset = { 0, 0 };
        renderingInfo.extent = { window->GetWidth(), window->GetHeight()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.colorAttachments = { colorAttachment };
        renderingInfo.depthAttachment = depthAttachment;
    
        auto commandList = device->GetCommandList(Core::QueueType::Graphics);
        commandList->Open();
    
        commandList->TransitionImageLayout(presentationImages[imageIndex], Core::Layout::ColorAttachment);
        commandList->TransitionImageLayout(depthImage, Core::Layout::DepthStencilAttachment);
    
        commandList->BeginRendering(renderingInfo);
    
        commandList->SetViewport(0.f, 0.f, static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
        commandList->SetScissor(0, 0, window->GetWidth(), window->GetHeight());
    
        commandList->BindPipeline(unlitPipeline);
    
        commandList->BindDescriptorSet(globalPipelineLayout, "frameData", frameDataDescriptorSet, Core::PipelineBindPoint::Graphics);
        commandList->BindDescriptorSet(globalPipelineLayout, "drawData", drawDataDescriptorSet, Core::PipelineBindPoint::Graphics);
    
        commandList->BindVertexBuffer(vertexBuffer, 0);
        commandList->BindIndexBuffer(indexBuffer, 0);
        
        commandList->DrawIndexed(static_cast<uint32_t>(meshIndices.size()), 1, 0, 0, 0);
    
        commandList->EndRendering();
    
        commandList->TransitionImageLayout(presentationImages[imageIndex], Core::Layout::Present);
    
        commandList->Close();
    
        uint64_t signalValue = frameCounter + 1;
    
        Core::SubmitInfo submitInfo;
        submitInfo.waitSemaphores = { imageAvailablesSemaphores[syncIndex] };
        submitInfo.waitSemaphoresValues = { 0 };
        submitInfo.signalSemaphores = { frameTimelineSemaphore, renderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoresValues = { signalValue, 0 };
        submitInfo.stage = Core::PipelineStage::ColorOutput;
    
        device->SubmitCommandList(commandList, submitInfo);
    
        Core::PresentInfo presentInfo;
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
