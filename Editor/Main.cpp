#include <iostream>

#include <random>
#include <glm/gtc/constants.hpp>
#include <chrono>

#include "Window.hpp"
#include "WindowRenderer.hpp"
#include "../Engine/Code/Include/Engine/KEngine.hpp"
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
#include "CoreObject/StaticMesh.hpp"

#include "BuildMode.hpp"
#include "MeshBase.hpp"
#include "Texture2D.hpp"
#include "RHI/API/ComputePipeline.hpp"

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

    Core::KEngineSpecs engineSpecs;
    engineSpecs.windowAPI = Window::WindowAPI::SDL;
    engineSpecs.windowWidth = 720;
    engineSpecs.windowHeight = 480;
    engineSpecs.windowName = "KEngine";
    engineSpecs.allowWindowResize = true;
    engineSpecs.version = Core::Version(0.f, 0.f, 1.f);
    engineSpecs.rendererAPI = Core::RendererAPI::Vulkan;
    engineSpecs.debugLayers = { Core::DebugLayers::Validation };
    engineSpecs.priorityQueuesThreadsCount = { {ENGINE_QUEUE_ASSET, 2}, {ENGINE_QUEUE_PHYSICS, 1} };

    Core::KEngine::Init(engineSpecs);

    // Window Reference :
    Core::KSharedPtr<Window::Window> windowReference = Core::KEngine::Get().GetWindow();

    // ------------ Object loading test -----------------//
    //Core::KEngine::Get().CreateAsset("Engine/Assets/Meshes/viking_room.obj", "viking_room");
    //Core::KEngine::Get().CreateAsset("Engine/Assets/Textures/viking_room.png", "texture");

    AssetManager::KSoftAssetRef<CoreObject::StaticMesh> vikingRoom = Core::KEngine::Get().GetAsset<CoreObject::StaticMesh>("viking_room");
    AssetManager::KSoftAssetRef<CoreObject::Texture2D> texture = Core::KEngine::Get().GetAsset<CoreObject::Texture2D>("texture");

    Core::KSharedPtr<CoreObject::StaticMesh> vikingRoomMesh = vikingRoom.Get();
    Core::KSharedPtr<CoreObject::Texture2D> vikingRoomTexture = texture.Get();


    // TODO : move this here
    //Core::KEngine::Update();

    float aspectRatio = static_cast<float>(windowReference->GetWidth()) / static_cast<float>(windowReference->GetHeight( ));
    Core::EditorCamera editorCamera;
    editorCamera.SetPosition(glm::vec3(0, 10, 30.0f));
    editorCamera.SetRotation(glm::vec3(-15.f, 0.f, 0.f));
    editorCamera.SetFov(glm::radians(45.f));
    editorCamera.SetAspectRatio(aspectRatio);
    editorCamera.SetNearPlane(0.1f);
    editorCamera.SetFarPlane(100.0f);
    editorCamera.RecalculateMatrices();
    //glm::vec3 cameraPos = glm::vec3(0, 0, 5.0f);
    //glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    //glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);

    float fov = glm::radians(45.f);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    glm::mat4 proj = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    proj[1][1] *= -1;
    CameraData initCam;
    initCam.cameraVP = editorCamera.GetViewProjectionMatrix();
    initCam.cameraPos = editorCamera.GetPosition();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, -5.0f));
    model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.0f, 0, 0.0f));
    model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    Model mdl;
    mdl.model = glm::transpose(model);
    mdl.normal = normalMatrix;

    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();
    Core::SurfaceHandle surface = Core::KEngine::Get().GetWindowSurface();

    Core::SwapchainSpecs sSpecs;
    sSpecs.surface = surface;
    sSpecs.extent = Core::Extent2D(windowReference->GetWidth(), windowReference->GetHeight());
    sSpecs.imageCount = 2;
    sSpecs.presentMode = Core::PresentMode::Mailbox;
    sSpecs.imageFormat = Core::Format::BGRA8_SRGB;
    sSpecs.isDepthEnable = true;
    sSpecs.depthImageFormat = Core::Format::D32_SFLOAT;
    Core::SwapchainHandle swapchain = device->CreateSwapchain(sSpecs);

    // Mesh :
    Core::BufferHandle vertexBuffer = vikingRoomMesh->GetVertexBuffer();
    Core::BufferHandle indexBuffer = vikingRoomMesh->GetIndexBuffer();

    // Base Texture
    Core::ImageHandle textureImage = vikingRoomTexture->GetImage();

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

    //struct Input
    //{
    //    int a;
    //    int b;
    //};
    //
    //Core::BufferSpecs computeInput{};
    //computeInput.primaryUsage = Core::BufferUsage::Storage;
    //computeInput.size = sizeof(Input);
    //computeInput.memoryAccess = Core::MemoryAccess::Upload;
    //computeInput.pipelineStage = Core::PipelineStage::ComputeShader;
    //computeInput.isPersistentMapped = true;
    //Core::BufferHandle storageInput = device->CreateBuffer(computeInput);
    //
    //Core::BufferSpecs computeOutput{};
    //computeOutput.primaryUsage = Core::BufferUsage::RWStorage;
    //computeOutput.size = sizeof(int);
    //computeOutput.memoryAccess = Core::MemoryAccess::Readback;
    //computeOutput.pipelineStage = Core::PipelineStage::ComputeShader;
    //computeOutput.isPersistentMapped = true;
    //Core::BufferHandle storageOutput = device->CreateBuffer(computeOutput);
    //
    //Input compInput =
    //{
    //    .a = 60,
    //    .b = 9
    //};
    //
    //Core::ShaderHandle testComp = device->CreateShader("test.comp", Core::ShaderStage::Compute, false, false);
    //
    //std::vector<Core::DescriptorSetLayoutHandle> computeDescriptors = device->CreateDescriptorSetsLayouts(testComp);
    //Core::PipelineLayoutHandle computePipelineLayout = device->CreatePipelineLayout(computeDescriptors, {});
    //
    //Core::ComputePipelineSpecs computePipelineSpecs;
    //computePipelineSpecs.shader = testComp;
    //computePipelineSpecs.pipelineLayout = computePipelineLayout;
    //
    //Core::PipelineHandle computePipeline = device->CreateComputePipeline(computePipelineSpecs);
    //
    //Core::DescriptorSetLayoutHandle inputDataLayout = computePipelineLayout->GetDescriptorSetLayout("InputData");
    //Core::DescriptorSetLayoutHandle outputDataLayout = computePipelineLayout->GetDescriptorSetLayout("OutputData");
    //Core::DescriptorSetHandle inputDataDescriptorSet = device->CreateDescriptorSet(inputDataLayout);
    //Core::DescriptorSetHandle outputDataDescriptorSet = device->CreateDescriptorSet(outputDataLayout);
    //
    //device->SetDescriptorSetBuffer(inputDataDescriptorSet, "Particles", Core::DescriptorType::StorageBuffer, storageInput, 0, sizeof(Input));
    //device->SetDescriptorSetBuffer(outputDataDescriptorSet, "Result", Core::DescriptorType::StorageBuffer, storageOutput, 0, sizeof(int));
    //
    //Core::CommandListHandle copySSBODataCommandList = device->GetCommandList(Core::QueueType::Compute);
    //copySSBODataCommandList->Open();
    //
    //copySSBODataCommandList->SetBufferData(storageInput, &compInput, sizeof(Input), 0);
    //
    //copySSBODataCommandList->BindPipeline(computePipeline);
    //copySSBODataCommandList->BindDescriptorSet(computePipelineLayout, "InputData", inputDataDescriptorSet, Core::PipelineBindPoint::Compute);
    //copySSBODataCommandList->BindDescriptorSet(computePipelineLayout, "OutputData", outputDataDescriptorSet, Core::PipelineBindPoint::Compute);
    //
    //copySSBODataCommandList->Dispatch(1, 1, 1);
    //
    //copySSBODataCommandList->Close();
    //
    //Core::SubmitInfo compSubmitInfo;
    //compSubmitInfo.stage = Core::PipelineStage::ComputeShader;
    //device->SubmitCommandList(copySSBODataCommandList, compSubmitInfo);
    //
    //device->WaitIdle();
    //int* resultPtr = static_cast<int*>(storageOutput->GetMappedData());
    //int compResult = *resultPtr;
    //spdlog::info("Compute result: {}", compResult);

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

    Core::ShaderHandle particleVert = device->CreateShader("particle.vert", Core::ShaderStage::Vertex, false, true);

    Core::GraphicsPipelineSpecs unlitParticlePipelineSpecs;
    unlitParticlePipelineSpecs.colorAttachmentCount = 1;
    unlitParticlePipelineSpecs.colorAttachmentFormats = { swapchain->GetColorImageFormat() };
    unlitParticlePipelineSpecs.depthAttachment = swapchain->GetDepthImageFormat();
    unlitParticlePipelineSpecs.viewportCount = 1;
    unlitParticlePipelineSpecs.scissorCount = 1;
    unlitParticlePipelineSpecs.lineWidth = 1.f;
    unlitParticlePipelineSpecs.cullmode = Core::CullMode::Back;
    unlitParticlePipelineSpecs.frontFace = Core::FrontFace::CounterClockWise;
    unlitParticlePipelineSpecs.SamplesCount = Core::SampleCount::Count1;
    unlitParticlePipelineSpecs.blendColor = false;
    unlitParticlePipelineSpecs.depthTest = true;
    unlitParticlePipelineSpecs.depthWrite = true;
    unlitParticlePipelineSpecs.depthCompare = Core::CompareOp::LessOrEqual;
    unlitParticlePipelineSpecs.dynamicStates = { Core::DynamicState::ViewPort, Core::DynamicState::Scissor };
    unlitParticlePipelineSpecs.topology = Core::PrimitiveTopology::TriangleList;
    unlitParticlePipelineSpecs.shaders = { particleVert , unlitFrag };
    unlitParticlePipelineSpecs.pipelineLayout = globalPipelineLayout;

    Core::PipelineHandle unlitParticlePipeline = device->CreateGraphicsPipeline(unlitParticlePipelineSpecs);

    Core::DescriptorSetLayoutHandle particleDataLayout = globalPipelineLayout->GetDescriptorSetLayout("particleData");
    Core::DescriptorSetHandle oddParticleDataDescriptorSet = device->CreateDescriptorSet(particleDataLayout);
    Core::DescriptorSetHandle evenParticleDataDescriptorSet = device->CreateDescriptorSet(particleDataLayout);

    uint32_t nbParticles = 4096u;

    Core::BufferSpecs oddPositionsSpecs{};
    oddPositionsSpecs.primaryUsage = Core::BufferUsage::RWStorage;
    oddPositionsSpecs.size = 4 * sizeof(float) * nbParticles;
    oddPositionsSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    oddPositionsSpecs.pipelineStage = Core::PipelineStage::ComputeShader;
    oddPositionsSpecs.additionalStages = { Core::PipelineStage::VertexShader };
    oddPositionsSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    Core::BufferHandle oddPositions = device->CreateBuffer(oddPositionsSpecs);

    Core::BufferSpecs oddVelocitiesSpecs{};
    oddVelocitiesSpecs.primaryUsage = Core::BufferUsage::RWStorage;
    oddVelocitiesSpecs.size = 4 * sizeof(float) * nbParticles;
    oddVelocitiesSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    oddVelocitiesSpecs.pipelineStage = Core::PipelineStage::ComputeShader;
    oddVelocitiesSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    Core::BufferHandle oddVelocities = device->CreateBuffer(oddVelocitiesSpecs);

    Core::BufferSpecs evenPositionsSpecs{};
    evenPositionsSpecs.primaryUsage = Core::BufferUsage::RWStorage;
    evenPositionsSpecs.size = 4 * sizeof(float) * nbParticles;
    evenPositionsSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    evenPositionsSpecs.pipelineStage = Core::PipelineStage::ComputeShader;
    evenPositionsSpecs.additionalStages = { Core::PipelineStage::VertexShader };
    evenPositionsSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    Core::BufferHandle evenPositions = device->CreateBuffer(evenPositionsSpecs);

    Core::BufferSpecs evenVelocitiesSpecs{};
    evenVelocitiesSpecs.primaryUsage = Core::BufferUsage::RWStorage;
    evenVelocitiesSpecs.size = 4 * sizeof(float) * nbParticles;
    evenVelocitiesSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    evenVelocitiesSpecs.pipelineStage = Core::PipelineStage::ComputeShader;
    evenVelocitiesSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    Core::BufferHandle evenVelocities = device->CreateBuffer(evenVelocitiesSpecs);

    struct EffectData
    {
        float deltaTime;
        float gravity;
        uint32_t nbParticles;
    };

    Core::BufferSpecs effectDataSpecs{};
    effectDataSpecs.primaryUsage = Core::BufferUsage::Uniform;
    effectDataSpecs.size = sizeof(EffectData);
    effectDataSpecs.memoryAccess = Core::MemoryAccess::Upload;
    effectDataSpecs.pipelineStage = Core::PipelineStage::ComputeShader;
    effectDataSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    effectDataSpecs.isPersistentMapped = true;
    Core::BufferHandle effectDataUniform = device->CreateBuffer(effectDataSpecs);

    Core::ShaderHandle particleLifeComp = device->CreateShader("particleLife.comp", Core::ShaderStage::Compute, false, false);

    std::vector<Core::DescriptorSetLayoutHandle> particleDescriptors = device->CreateDescriptorSetsLayouts(particleLifeComp);
    Core::PipelineLayoutHandle particlePipelineLayout = device->CreatePipelineLayout(particleDescriptors, {});

    Core::ComputePipelineSpecs particlePipelineSpecs;
    particlePipelineSpecs.shader = particleLifeComp;
    particlePipelineSpecs.pipelineLayout = particlePipelineLayout;

    Core::PipelineHandle particlePipeline = device->CreateComputePipeline(particlePipelineSpecs);

    Core::DescriptorSetLayoutHandle particlesLayout = particlePipelineLayout->GetDescriptorSetLayout("particles");
    Core::DescriptorSetLayoutHandle effectDataLayout = particlePipelineLayout->GetDescriptorSetLayout("effectData");
    Core::DescriptorSetHandle oddParticlesDescriptorSet = device->CreateDescriptorSet(particlesLayout);
    Core::DescriptorSetHandle evenParticlesDescriptorSet = device->CreateDescriptorSet(particlesLayout);
    Core::DescriptorSetHandle effectDataDescriptorSet = device->CreateDescriptorSet(effectDataLayout);

    device->SetDescriptorSetBuffer(oddParticlesDescriptorSet, "inPositions", Core::DescriptorType::StorageBuffer, evenPositions, 0, 4 * sizeof(float) * nbParticles);
    device->SetDescriptorSetBuffer(oddParticlesDescriptorSet, "inVelocities", Core::DescriptorType::StorageBuffer, evenVelocities, 0, 4 * sizeof(float) * nbParticles);
    device->SetDescriptorSetBuffer(oddParticlesDescriptorSet, "outPositions", Core::DescriptorType::StorageBuffer, oddPositions, 0, 4 * sizeof(float) * nbParticles);
    device->SetDescriptorSetBuffer(oddParticlesDescriptorSet, "outVelocities", Core::DescriptorType::StorageBuffer, oddVelocities, 0, 4 * sizeof(float) * nbParticles);

    device->SetDescriptorSetBuffer(evenParticlesDescriptorSet, "inPositions", Core::DescriptorType::StorageBuffer, oddPositions, 0, 4 * sizeof(float) * nbParticles);
    device->SetDescriptorSetBuffer(evenParticlesDescriptorSet, "inVelocities", Core::DescriptorType::StorageBuffer, oddVelocities, 0, 4 * sizeof(float) * nbParticles);
    device->SetDescriptorSetBuffer(evenParticlesDescriptorSet, "outPositions", Core::DescriptorType::StorageBuffer, evenPositions, 0, 4 * sizeof(float) * nbParticles);
    device->SetDescriptorSetBuffer(evenParticlesDescriptorSet, "outVelocities", Core::DescriptorType::StorageBuffer, evenVelocities, 0, 4 * sizeof(float) * nbParticles);

    device->SetDescriptorSetBuffer(effectDataDescriptorSet, "globalUniform", Core::DescriptorType::UniformBuffer, effectDataUniform, 0, sizeof(EffectData));

    device->SetDescriptorSetBuffer(oddParticleDataDescriptorSet, "positions", Core::DescriptorType::StorageBuffer, oddPositions, 0, 4 * sizeof(float) * nbParticles);
    device->SetDescriptorSetBuffer(evenParticleDataDescriptorSet, "positions", Core::DescriptorType::StorageBuffer, evenPositions, 0, 4 * sizeof(float) * nbParticles);

    Core::DrawIndexedIndirectCommand particleDrawCommand{};
    particleDrawCommand.indexCount = vikingRoomMesh->GetIndicesCount();
    particleDrawCommand.instanceCount = nbParticles;
    particleDrawCommand.firstIndex = 0;
    particleDrawCommand.vertexOffset = 0;
    particleDrawCommand.firstInstance = 0;

    Core::BufferSpecs indirectSpecs{};
    indirectSpecs.primaryUsage = Core::BufferUsage::DrawIndirect;
    indirectSpecs.size = sizeof(Core::DrawIndexedIndirectCommand);
    indirectSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    indirectSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    indirectSpecs.pipelineStage = Core::PipelineStage::Indirect;
    
    Core::BufferHandle indirectBuffer = device->CreateBuffer(indirectSpecs);

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> distUnit(0.0f, 1.0f);
    std::uniform_real_distribution<float> distSpeed(1.0f, 15.0f);

    std::vector<glm::vec4> positionsVector;
    std::vector<glm::vec4> velocitiesVector;
    positionsVector.reserve(nbParticles);
    velocitiesVector.reserve(nbParticles);
    for (int i = 0; i < static_cast<int>(nbParticles); ++i)
    {
        glm::vec4 pos = glm::vec4(0, 2, -3, 1);
        positionsVector.push_back(pos);

        float theta = distUnit(rng) * glm::two_pi<float>();
        float cosPhi = 1.0f - 2.0f * distUnit(rng);
        float sinPhi = std::sqrt(1.0f - cosPhi * cosPhi);

        glm::vec3 dir(sinPhi * std::cos(theta), cosPhi, sinPhi * std::sin(theta));
        float speed = distSpeed(rng);

        velocitiesVector.push_back(glm::vec4(dir * speed, 0.0f));
    }

    Core::CommandListHandle copyParticlesCommandList = device->GetCommandList(Core::QueueType::Transfer);

    copyParticlesCommandList->Open();

    copyParticlesCommandList->SetBufferData(indirectBuffer, &particleDrawCommand, sizeof(Core::DrawIndexedIndirectCommand), 0);
    
    copyParticlesCommandList->SetBufferData(oddPositions, positionsVector.data(), 4 * sizeof(float) * nbParticles, 0);
    copyParticlesCommandList->SetBufferData(evenPositions, positionsVector.data(), 4 * sizeof(float) * nbParticles, 0);
    copyParticlesCommandList->SetBufferData(oddVelocities, velocitiesVector.data(), 4 * sizeof(float) * nbParticles, 0);
    copyParticlesCommandList->SetBufferData(evenVelocities, velocitiesVector.data(), 4 * sizeof(float) * nbParticles, 0);

    copyParticlesCommandList->Close();

    Core::SubmitInfo submitInfoCopyParticles;
    submitInfoCopyParticles.stage = Core::PipelineStage::Transfer;

    device->SubmitCommandList(copyParticlesCommandList, submitInfoCopyParticles);

    device->WaitIdle();
    uint64_t frameCounter = 0;
    bool gpuResizeRequest = false;
    float deltaTime = 0.f;
    using Clock = std::chrono::high_resolution_clock;
    auto lastFrameTime = Clock::now();
    bool isFrameEven = false;

    while (!windowReference->ShouldClose())
    {
        isFrameEven = (frameCounter % 2 == 0);
        auto currentFrameTime = Clock::now();
        deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;
        windowReference->PollEvents();
        //Core::KEngine::Update();

        if (windowReference->GetHasResize() || gpuResizeRequest)
        {
            device->WaitIdle();

            //// Destroy everything.
            device->DestroyPresentationImages(presentationImages);
            device->DestroyImage(depthImage);
            device->DestroySwapchain(swapchain);

            //// Update everything
            device->UpdateCompatibility(surface);

            // Recreate everything.
            uint32_t tWidth = windowReference->GetWidth();
            uint32_t tHeight = windowReference->GetHeight();
            sSpecs.extent = Core::Extent2D(tWidth, tHeight);
            swapchain = device->CreateSwapchain(sSpecs);
            presentationImages = device->CreatePresentationImages(swapchain);
            depthImage = device->CreateImagesWithSwapchain(depthImageSpecs, swapchain);

            windowReference->ResizeComplete();
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
            device->SetDescriptorSetBuffer(frameDataDescriptorSet, "camera", Core::DescriptorType::UniformBuffer, uniformCamera, 0, sizeof(CameraData));
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

        EffectData effData{};
        effData.deltaTime = deltaTime;
        effData.gravity = 9.81f;
        effData.nbParticles = nbParticles;

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
        renderingInfo.extent = { windowReference->GetWidth(), windowReference->GetHeight()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.colorAttachments = { colorAttachment };
        renderingInfo.depthAttachment = depthAttachment;
        auto commandList = device->GetCommandList(Core::QueueType::Graphics);
        commandList->Open();

        commandList->SetBufferData(effectDataUniform, &effData, sizeof(EffectData), 0);

        commandList->BindPipeline(particlePipeline);

        Core::DescriptorSetHandle particleDesc = isFrameEven ? evenParticlesDescriptorSet : oddParticlesDescriptorSet;
        commandList->BindDescriptorSet(particlePipelineLayout, "particles", particleDesc, Core::PipelineBindPoint::Compute);
        commandList->BindDescriptorSet(particlePipelineLayout, "effectData", effectDataDescriptorSet, Core::PipelineBindPoint::Compute);

        commandList->Dispatch(16, 1, 1);

        commandList->TransitionImageLayout(presentationImages[imageIndex], Core::Layout::ColorAttachment);
        commandList->TransitionImageLayout(depthImage, Core::Layout::DepthStencilAttachment);

        commandList->BeginRendering(renderingInfo);

        commandList->SetViewport(0.f, 0.f, static_cast<float>(windowReference->GetWidth()), static_cast<float>(windowReference->GetHeight()));
        commandList->SetScissor(0, 0, windowReference->GetWidth(), windowReference->GetHeight());

        commandList->BindPipeline(unlitPipeline);

        commandList->BindDescriptorSet(globalPipelineLayout, "frameData", frameDataDescriptorSet, Core::PipelineBindPoint::Graphics);
        commandList->BindDescriptorSet(globalPipelineLayout, "drawData", drawDataDescriptorSet, Core::PipelineBindPoint::Graphics);

        commandList->BindVertexBuffer(vertexBuffer, 0);
        commandList->BindIndexBuffer(indexBuffer, 0);

        commandList->DrawIndexed(static_cast<uint32_t>(vikingRoomMesh->GetIndicesCount()), 1, 0, 0, 0);

        commandList->BindPipeline(unlitParticlePipeline);

        commandList->BindDescriptorSet(globalPipelineLayout, "frameData", frameDataDescriptorSet, Core::PipelineBindPoint::Graphics);
        commandList->BindDescriptorSet(globalPipelineLayout, "drawData", drawDataDescriptorSet, Core::PipelineBindPoint::Graphics);

        Core::DescriptorSetHandle particleRenderSet = isFrameEven ? evenParticleDataDescriptorSet : oddParticleDataDescriptorSet;
        commandList->BindDescriptorSet(globalPipelineLayout, "particleData", particleRenderSet, Core::PipelineBindPoint::Graphics);

        commandList->BindVertexBuffer(vertexBuffer, 0);
        commandList->BindIndexBuffer(indexBuffer, 0);

        commandList->DrawIndexedIndirect(indirectBuffer, 0, 1, sizeof(Core::DrawIndexedIndirectCommand));

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

    device->DestroySampler(sampler);
    device->DestroyBuffer(uniformCamera);
    device->DestroyBuffer(uniformModel);

    device->DestroyDescriptorSet(frameDataDescriptorSet);
    device->DestroyDescriptorSet(drawDataDescriptorSet);

    device->DestroyBuffer(oddPositions);
    device->DestroyBuffer(oddVelocities);
    device->DestroyBuffer(evenPositions);
    device->DestroyBuffer(evenVelocities);
    device->DestroyBuffer(effectDataUniform);
    device->DestroyBuffer(indirectBuffer);
    device->DestroyDescriptorSet(oddParticleDataDescriptorSet);
    device->DestroyDescriptorSet(evenParticleDataDescriptorSet);
    device->DestroyDescriptorSet(oddParticlesDescriptorSet);
    device->DestroyDescriptorSet(evenParticlesDescriptorSet);
    device->DestroyDescriptorSet(effectDataDescriptorSet);
    device->DestroyPipeline(unlitParticlePipeline);
    device->DestroyPipeline(particlePipeline);
    device->DestroyPipelineLayout(particlePipelineLayout);
    device->DestroyDescriptorSetsLayouts(particleDescriptors);
    device->DestroyShader(particleLifeComp);
    device->DestroyShader(particleVert);

    device->DestroyPipeline(unlitPipeline);
    device->DestroyPipelineLayout(globalPipelineLayout);
    device->DestroyDescriptorSetsLayouts(globalLayoutDescriptors);

    //device->DestroyBuffer(storageInput);
    //device->DestroyBuffer(storageOutput);
    //device->DestroyDescriptorSet(inputDataDescriptorSet);
    //device->DestroyDescriptorSet(outputDataDescriptorSet);
    //device->DestroyPipeline(computePipeline);
    //device->DestroyPipelineLayout(computePipelineLayout);
    //device->DestroyDescriptorSetsLayouts(computeDescriptors);
    //device->DestroyShader(testComp);

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

    device->DestroySwapchain(swapchain);

    Core::KEngine::Clean();
    Core::KEngine::Shutdown();

#ifdef KDEBUG
    do
    {
        std::cout << '\n' << "Press enter to continue...";
    } while (std::cin.get() != '\n');
#endif

    return 0;
}
