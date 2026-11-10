#include <iostream>

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
#include "Memory/SharedPtr.hpp"
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

namespace Kayou::Core
{
    struct TestObject
    {
        static inline int aliveCount = 0;
        static inline int destroyedCount = 0;
        int value = 0;

        TestObject(int val = 0) : value(val) { aliveCount++; }
        ~TestObject() { aliveCount--; destroyedCount++; }

        static void ResetStats()
        {
            aliveCount = 0;
            destroyedCount = 0;
        }
    };

    struct BaseObject
    {
        virtual ~BaseObject() = default;
        int baseValue = 42;
    };

    struct DerivedObject : public BaseObject
    {
        int derivedValue = 99;
    };

    void Test_BasicConstruction()
    {
        TestObject::ResetStats();

        // Default & Nullptr
        SharedPtr<TestObject> p1;
        SharedPtr<TestObject> p2(nullptr);
        assert(p1.Get() == nullptr);
        assert(p2.Get() == nullptr);
        assert(!p1 && !p2);

        // Raw pointer allocation
        {
            SharedPtr<TestObject> p3(new TestObject(10));
            assert(p3.Get() != nullptr);
            assert(p3->value == 10);
            assert((*p3).value == 10);
            assert(p3.GetRefCount() == 1);
            assert(TestObject::aliveCount == 1);
        }
        // Sortie du scope -> Libération automatique
        assert(TestObject::aliveCount == 0);
        assert(TestObject::destroyedCount == 1);

        std::cout << "[PASS] Basic Construction & Destruction\n";
    }

    void Test_CopySemantics()
    {
        TestObject::ResetStats();

        {
            Core::SharedPtr<TestObject> p1(new TestObject(100));
            assert(p1.GetRefCount() == 1);

            {
                Core::SharedPtr<TestObject> p2 = p1; // Copy Constructor
                assert(p1.Get() == p2.Get());
                assert(p1.GetRefCount() == 2);
                assert(p2.GetRefCount() == 2);

                Core::SharedPtr<TestObject> p3;
                p3 = p2; // Copy Assignment
                assert(p1.GetRefCount() == 3);
            }
            // p2 et p3 détruits -> refCount doit revenir à 1
            assert(p1.GetRefCount() == 1);
            assert(TestObject::aliveCount == 1);
        }
        // p1 détruit -> objet supprimé
        assert(TestObject::aliveCount == 0);
        assert(TestObject::destroyedCount == 1);

        std::cout << "[PASS] Copy Semantics & Reference Counting\n";
    }

    void Test_MoveSemantics()
    {
        TestObject::ResetStats();

        {
            Core::SharedPtr<TestObject> p1(new TestObject(500));
            TestObject* rawPtr = p1.Get();

            SharedPtr<TestObject> p2 = std::move(p1); // Move Constructor
            assert(p1.Get() == nullptr); // NOLINT
            assert(p1.GetRefCount() == 0);
            assert(p2.Get() == rawPtr);
            assert(p2.GetRefCount() == 1);

            SharedPtr<TestObject> p3;
            p3 = std::move(p2); // Move Assignment
            assert(p2.Get() == nullptr); // NOLINT
            assert(p3.Get() == rawPtr);
            assert(p3.GetRefCount() == 1);
        }
        assert(TestObject::aliveCount == 0);

        std::cout << "[PASS] Move Semantics\n";
    }

    void Test_Polymorphism()
    {
        {
            SharedPtr<DerivedObject> derived(new DerivedObject());
            assert(derived.GetRefCount() == 1);

            // Conversion Copy Constructor
            SharedPtr<BaseObject> base = derived;
            assert(base.GetRefCount() == 2);
            assert(base->baseValue == 42);

            // Conversion Move Constructor
            SharedPtr<BaseObject> base2 = std::move(derived);
            assert(derived.Get() == nullptr); // NOLINT
            assert(base.GetRefCount() == 2);
        }

        std::cout << "[PASS] Polymorphism & Conversions\n";
    }

    void Test_AliasingConstructor()
    {
        TestObject::ResetStats();

        {
            SharedPtr<TestObject> owner(new TestObject(777));

            // Pointeur d'aliasing vers un membre interne (ici `owner->value`)
            SharedPtr<int> aliasPtr(owner, &owner->value);

            assert(aliasPtr.Get() == &owner->value);
            assert(*aliasPtr == 777);
            assert(owner.GetRefCount() == 2);
            assert(aliasPtr.GetRefCount() == 2);

            // On libère le pointeur d'origine : l'objet ne doit PAS être détruit
            owner.Reset();
            assert(aliasPtr.GetRefCount() == 1);
            assert(TestObject::aliveCount == 1);
            assert(*aliasPtr == 777);
        }
        // Destruction du aliasPtr -> l'objet hôte doit enfin être libéré
        assert(TestObject::aliveCount == 0);
        assert(TestObject::destroyedCount == 1);

        std::cout << "[PASS] Aliasing Constructor & Partial Destruction\n";
    }

    void Test_EdgeCases()
    {
        TestObject::ResetStats();

        // Self-Assignment test
        {
            SharedPtr<TestObject> p1(new TestObject(42));
            p1 = std::as_const(p1); // N'interrompt pas le refCount
            assert(p1.GetRefCount() == 1);
            assert(p1.Get() != nullptr);

            // Reset manuel
            p1.Reset();
            assert(p1.Get() == nullptr);
            assert(p1.GetRefCount() == 0);
            assert(TestObject::aliveCount == 0);

            // Assignation nullptr
            SharedPtr<TestObject> p2(new TestObject(99));
            p2 = nullptr;
            assert(p2.Get() == nullptr);
            assert(TestObject::aliveCount == 0);
        }

        std::cout << "[PASS] Edge Cases (Self-Assignment & Reset)\n";
    }
}
int main()
{
#ifdef KDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    Core::Test_BasicConstruction();
    Core::Test_CopySemantics();
    Core::Test_MoveSemantics();
    //Core::Test_Polymorphism();
    //Core::Test_AliasingConstructor();
    //Core::Test_EdgeCases();


    //Core::KEngineSpecs engineSpecs;
    //engineSpecs.windowAPI = Window::WindowAPI::SDL;engineSpecs.windowWidth = 720;engineSpecs.windowHeight = 480;engineSpecs.windowName = "KEngine";engineSpecs.allowWindowResize = true;engineSpecs.version = Core::Version(0.f, 0.f, 1.f);engineSpecs.rendererAPI = Core::RendererAPI::Vulkan;engineSpecs.debugLayers = { Core::DebugLayers::Validation };engineSpecs.priorityQueuesThreadsCount = { {ENGINE_QUEUE_ASSET, 2}, {ENGINE_QUEUE_PHYSICS, 1} };Core::KEngine::Init(engineSpecs);// Window Reference :Core::KSharedPtr<Window::Window> windowReference = Core::KEngine::Get().GetWindow();// ------------ Object loading test -----------------////Core::KEngine::Get().CreateAsset("Engine/Assets/Meshes/viking_room.obj", "viking_room");//Core::KEngine::Get().CreateAsset("Engine/Assets/Textures/viking_room.png", "texture");AssetManager::KSoftAssetRef<CoreObject::StaticMesh> vikingRoom = Core::KEngine::Get().GetAsset<CoreObject::StaticMesh>("viking_room");AssetManager::KSoftAssetRef<CoreObject::Texture2D> texture = Core::KEngine::Get().GetAsset<CoreObject::Texture2D>("texture");Core::KSharedPtr<CoreObject::StaticMesh> vikingRoomMesh = vikingRoom.Get();Core::KSharedPtr<CoreObject::Texture2D> vikingRoomTexture = texture.Get();// TODO : move this here//Core::KEngine::Update();float aspectRatio = static_cast<float>(windowReference->GetWidth()) / static_cast<float>(windowReference->GetHeight( ));Core::EditorCamera editorCamera;editorCamera.SetPosition(glm::vec3(0, 0, 5.0f));editorCamera.SetRotation(glm::vec3(0, 0, 0.f));editorCamera.SetFov(glm::radians(45.f));editorCamera.SetAspectRatio(aspectRatio);editorCamera.SetNearPlane(0.1f);editorCamera.SetFarPlane(100.0f);editorCamera.RecalculateMatrices();//glm::vec3 cameraPos = glm::vec3(0, 0, 5.0f);//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);//glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);//glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);float fov = glm::radians(45.f);float nearPlane = 0.1f;float farPlane = 100.0f;glm::mat4 proj = glm::perspective(fov, aspectRatio, nearPlane, farPlane);proj[1][1] *= -1;CameraData initCam;initCam.cameraVP = editorCamera.GetViewProjectionMatrix();initCam.cameraPos = editorCamera.GetPosition();glm::mat4 model = glm::mat4(1.0f);model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.0f, 0, 0.0f));model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));Model mdl;mdl.model = glm::transpose(model);mdl.normal = normalMatrix;Core::DeviceHandle device = Core::KEngine::Get().GetDevice();Core::SurfaceHandle surface = Core::KEngine::Get().GetWindowSurface();Core::SwapchainSpecs sSpecs;sSpecs.surface = surface;sSpecs.extent = Core::Extent2D(windowReference->GetWidth(), windowReference->GetHeight());sSpecs.imageCount = 2;sSpecs.presentMode = Core::PresentMode::Mailbox;sSpecs.imageFormat = Core::Format::BGRA8_SRGB;sSpecs.isDepthEnable = true;sSpecs.depthImageFormat = Core::Format::D32_SFLOAT;Core::SwapchainHandle swapchain = device->CreateSwapchain(sSpecs);// Mesh :Core::BufferHandle vertexBuffer = vikingRoomMesh->GetVertexBuffer();Core::BufferHandle indexBuffer = vikingRoomMesh->GetIndexBuffer();// Base TextureCore::ImageHandle textureImage = vikingRoomTexture->GetImage();// Base SamplerCore::SamplerSpecs samplerSpecs;samplerSpecs.magFilter = Core::Filter::Linear;samplerSpecs.minFilter = Core::Filter::Linear;samplerSpecs.mipmapMode = Core::SamplerMipmapMode::Linear;samplerSpecs.addressU = Core::SamplerAddressMode::Repeat;samplerSpecs.addressV = Core::SamplerAddressMode::Repeat;samplerSpecs.addressW = Core::SamplerAddressMode::Repeat;samplerSpecs.mipLodBias = 0.0f;samplerSpecs.anisotropyEnable = false;samplerSpecs.maxAnisotropy = 1.0f;samplerSpecs.compareEnable = false;samplerSpecs.compareOp = Core::CompareOp::Always;samplerSpecs.minLod = 0.0f;samplerSpecs.maxLod = 0.0f;samplerSpecs.borderColor = Core::BorderColor::IntOpaqueBlack;samplerSpecs.unnormalizedCoordinates = false;Core::SamplerHandle sampler = device->CreateSampler(samplerSpecs);
//
    //// Uniform buffer camera
    //Core::BufferSpecs uniformCameraSpecs{};
    //uniformCameraSpecs.primaryUsage = Core::BufferUsage::Uniform;
    //uniformCameraSpecs.size = sizeof(CameraData);
    //uniformCameraSpecs.memoryAccess = Core::MemoryAccess::Upload;
    //uniformCameraSpecs.pipelineStage = Core::PipelineStage::VertexShader;
    //uniformCameraSpecs.isPersistentMapped = true;
    //uniformCameraSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    //Core::BufferHandle uniformCamera = device->CreateBuffer(uniformCameraSpecs);
//
//
    // Uniform buffer modelMatrix
    //Core::BufferSpecs uniformModelSpecs{};
    //uniformModelSpecs.primaryUsage = Core::BufferUsage::Uniform;
    //uniformModelSpecs.size = sizeof(Model);
    //uniformModelSpecs.memoryAccess = Core::MemoryAccess::Upload;
    //uniformModelSpecs.pipelineStage = Core::PipelineStage::VertexShader;
    //uniformModelSpecs.isPersistentMapped = true;
    //uniformModelSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    //Core::BufferHandle uniformModel = device->CreateBuffer(uniformModelSpecs);
//
    //auto copyUniformDataCommandList = device->GetCommandList(Core::QueueType::Graphics);
    //copyUniformDataCommandList->Open();
//
    //copyUniformDataCommandList->SetBufferData(uniformCamera, &initCam, sizeof(CameraData), 0);
    //copyUniformDataCommandList->SetBufferData(uniformModel, &mdl, sizeof(Model), 0);
//
    //copyUniformDataCommandList->Close();
//
    //Core::SubmitInfo submitInfocopyUniformData;
    //submitInfocopyUniformData.stage = Core::PipelineStage::Transfer;
//
    //device->SubmitCommandList(copyUniformDataCommandList, submitInfocopyUniformData);
//
    //// Presentation images
    //std::vector<Core::ImageHandle> presentationImages = device->CreatePresentationImages(swapchain);
//
    //// depth images
    //Core::SwapchainImageSpecs depthImageSpecs;
    //depthImageSpecs.imageType = Core::SwapchainImageType::Depth;
    //depthImageSpecs.targetLayout = Core::Layout::DepthStencilAttachment;
    //depthImageSpecs.type = Core::ImageType::Image2D;
    //depthImageSpecs.usages = { Core::ImageUsage::DepthStencilAttachment };
    //depthImageSpecs.viewType = Core::ImageViewType::Image2D;
    //depthImageSpecs.viewAspect = Core::ImageViewAspect::Depth;
//
    //Core::ImageHandle depthImage = device->CreateImagesWithSwapchain(depthImageSpecs, swapchain);
//
    //// Timeline
    //Core::SemaphoreSpecs timelineSpecs;
    //timelineSpecs.type = Core::SemaphoreType::Timeline;
    //Core::SemaphoreHandle frameTimelineSemaphore = device->CreateSemaphore(timelineSpecs);
//
    //// Binary
    //std::vector<Core::SemaphoreHandle> imageAvailablesSemaphores;
    //std::vector<Core::SemaphoreHandle> renderFinishedSemaphores;
//
    //for (uint32_t i = 0; i < swapchain->GetImageCount(); ++i)
    //{
    //    Core::SemaphoreSpecs binarySpecs{};
    //    binarySpecs.type = Core::SemaphoreType::Binary;
//
    //    Core::SemaphoreHandle imageAvailableSemaphore = device->CreateSemaphore(binarySpecs);
    //    Core::SemaphoreHandle renderFinishedSemaphore = device->CreateSemaphore(binarySpecs);
//
    //    imageAvailablesSemaphores.push_back(imageAvailableSemaphore);
    //    renderFinishedSemaphores.push_back(renderFinishedSemaphore);
    //}
//
    //Core::ShaderHandle baseVert = device->CreateShader("base.vert", Core::ShaderStage::Vertex, false, true);
    //Core::ShaderHandle unlitFrag = device->CreateShader("unlit.frag", Core::ShaderStage::Fragment, false, true);
    //Core::ShaderHandle globalLayoutShader = device->CreateShader("globalLayout", Core::ShaderStage::Vertex, true, true);
//
    //std::vector<Core::DescriptorSetLayoutHandle> globalLayoutDescriptors = device->CreateDescriptorSetsLayouts(globalLayoutShader);
//
    //Core::PipelineLayoutHandle globalPipelineLayout = device->CreatePipelineLayout(globalLayoutDescriptors, {});
//
    //Core::GraphicsPipelineSpecs unlitPipelineSpecs;
    //unlitPipelineSpecs.colorAttachmentCount = 1;
    //unlitPipelineSpecs.colorAttachmentFormats = { swapchain->GetColorImageFormat() };
    //unlitPipelineSpecs.depthAttachment = swapchain->GetDepthImageFormat();
    //unlitPipelineSpecs.viewportCount = 1;
    //unlitPipelineSpecs.scissorCount = 1;
    //unlitPipelineSpecs.lineWidth = 1.f;
    //unlitPipelineSpecs.cullmode = Core::CullMode::Back;
    //unlitPipelineSpecs.frontFace = Core::FrontFace::CounterClockWise;
    //unlitPipelineSpecs.SamplesCount = Core::SampleCount::Count1;
    //unlitPipelineSpecs.blendColor = false;
    //unlitPipelineSpecs.depthTest = true;
    //unlitPipelineSpecs.depthWrite = true;
    //unlitPipelineSpecs.depthCompare = Core::CompareOp::LessOrEqual;
    //unlitPipelineSpecs.dynamicStates = { Core::DynamicState::ViewPort, Core::DynamicState::Scissor };
    //unlitPipelineSpecs.topology = Core::PrimitiveTopology::TriangleList;
    //unlitPipelineSpecs.shaders = { baseVert , unlitFrag };
    //unlitPipelineSpecs.pipelineLayout = globalPipelineLayout;
//
    //Core::PipelineHandle unlitPipeline = device->CreateGraphicsPipeline(unlitPipelineSpecs);
//
    //// DescriptorSet
    //Core::DescriptorSetLayoutHandle frameDataLayout = globalPipelineLayout->GetDescriptorSetLayout("frameData");
    //Core::DescriptorSetLayoutHandle drawDataLayout = globalPipelineLayout->GetDescriptorSetLayout("drawData");
//
    //Core::DescriptorSetHandle frameDataDescriptorSet = device->CreateDescriptorSet(frameDataLayout);
    //Core::DescriptorSetHandle drawDataDescriptorSet = device->CreateDescriptorSet(drawDataLayout);
//
    //device->SetDescriptorSetBuffer(frameDataDescriptorSet, "camera", Core::DescriptorType::UniformBuffer, uniformCamera, 0, sizeof(CameraData));
    //device->SetDescriptorSetBuffer(drawDataDescriptorSet, "object", Core::DescriptorType::UniformBuffer, uniformModel, 0, sizeof(Model));
    //device->SetDescriptorSetImage(drawDataDescriptorSet, "texture2D", Core::DescriptorType::SampledImage, textureImage, nullptr);
    //device->SetDescriptorSetSampler(drawDataDescriptorSet, "sampler", Core::DescriptorType::Sampler, sampler);
//
    //device->WaitIdle();
    //uint64_t frameCounter = 0;
    //bool gpuResizeRequest = false;
//
    //while (!windowReference->ShouldClose())
    //{
    //    windowReference->PollEvents();
    //    //Core::KEngine::Update();
//
    //    if (windowReference->GetHasResize() || gpuResizeRequest)
    //    {
    //        device->WaitIdle();
//
    //        //// Destroy everything.
    //        device->DestroyPresentationImages(presentationImages);
    //        device->DestroyImage(depthImage);
    //        device->DestroySwapchain(swapchain);
//
    //        //// Update everything
    //        device->UpdateCompatibility(surface);
//
    //        // Recreate everything.
    //        uint32_t tWidth = windowReference->GetWidth();
    //        uint32_t tHeight = windowReference->GetHeight();
    //        sSpecs.extent = Core::Extent2D(tWidth, tHeight);
    //        swapchain = device->CreateSwapchain(sSpecs);
    //        presentationImages = device->CreatePresentationImages(swapchain);
    //        depthImage = device->CreateImagesWithSwapchain(depthImageSpecs, swapchain);
//
    //        windowReference->ResizeComplete();
    //        gpuResizeRequest = false;
//
    //        editorCamera.SetAspectRatio(static_cast<float>(tWidth) / static_cast<float>(tHeight));
    //        editorCamera.RecalculateMatrices();
//
    //        CameraData resizeCam;
    //        resizeCam.cameraVP = editorCamera.GetViewProjectionMatrix();
    //        resizeCam.cameraPos = editorCamera.GetPosition();
//
    //        auto commandList = device->GetCommandList(Core::QueueType::Graphics);
    //        commandList->Open();
    //        commandList->SetBufferData(uniformCamera, &resizeCam, sizeof(CameraData), 0);
    //        commandList->Close();
//
    //        Core::SubmitInfo submitTransfer;
    //        Core::SubmitInfo submitInfo;
    //        submitInfo.stage = Core::PipelineStage::None;
//
    //        device->SubmitCommandList(commandList, submitTransfer);
    //        device->SetDescriptorSetBuffer(frameDataDescriptorSet, "camera", Core::DescriptorType::UniformBuffer, uniformCamera, 0, sizeof(CameraData));
    //    }
//
    //    uint32_t maxFramesInFlight = swapchain->GetImageCount();
    //    uint32_t syncIndex = frameCounter % maxFramesInFlight;
//
    //    if (frameCounter >= maxFramesInFlight)
    //    {
    //        uint64_t waitValue = frameCounter - maxFramesInFlight + 1;
    //        device->WaitForSemaphore(frameTimelineSemaphore, waitValue);
    //    }
//
    //    uint32_t imageIndex = device->AcquirreNextImage(swapchain, imageAvailablesSemaphores[syncIndex]);
//
    //    device->RunGarbageCollector();
//
    //    Core::RenderingAttachment colorAttachment;
    //    colorAttachment.image = presentationImages[imageIndex];
    //    colorAttachment.layout = Core::Layout::ColorAttachment;
    //    colorAttachment.loadOp = Core::LoadOp::Clear;
    //    colorAttachment.storeOp = Core::StoreOp::Store;
    //    colorAttachment.clearValueColor = Core::ClearValue(0.1f, 0.1f, 0.1f, 1.0f);
//
    //    Core::RenderingAttachment depthAttachment;
    //    depthAttachment.image = depthImage;
    //    depthAttachment.layout = Core::Layout::DepthStencilAttachment;
    //    depthAttachment.loadOp = Core::LoadOp::Clear;
    //    depthAttachment.storeOp = Core::StoreOp::Store;
    //    depthAttachment.clearValueDepth = Core::ClearValue(1.0f, 0.f, 0.f, 0.f);
//
    //    Core::RenderingInfo renderingInfo;
    //    renderingInfo.offset = { 0, 0 };
    //    renderingInfo.extent = { windowReference->GetWidth(), windowReference->GetHeight()};
    //    renderingInfo.layerCount = 1;
    //    renderingInfo.colorAttachmentCount = 1;
    //    renderingInfo.colorAttachments = { colorAttachment };
    //    renderingInfo.depthAttachment = depthAttachment;
    //    auto commandList = device->GetCommandList(Core::QueueType::Graphics);
    //    commandList->Open();
//
    //    commandList->TransitionImageLayout(presentationImages[imageIndex], Core::Layout::ColorAttachment);
    //    commandList->TransitionImageLayout(depthImage, Core::Layout::DepthStencilAttachment);
//
    //    commandList->BeginRendering(renderingInfo);
//
    //    commandList->SetViewport(0.f, 0.f, static_cast<float>(windowReference->GetWidth()), static_cast<float>(windowReference->GetHeight()));
    //    commandList->SetScissor(0, 0, windowReference->GetWidth(), windowReference->GetHeight());
//
    //    commandList->BindPipeline(unlitPipeline);
//
    //    commandList->BindDescriptorSet(globalPipelineLayout, "frameData", frameDataDescriptorSet, Core::PipelineBindPoint::Graphics);
    //    commandList->BindDescriptorSet(globalPipelineLayout, "drawData", drawDataDescriptorSet, Core::PipelineBindPoint::Graphics);
//
    //    commandList->BindVertexBuffer(vertexBuffer, 0);
    //    commandList->BindIndexBuffer(indexBuffer, 0);
//
    //    commandList->DrawIndexed(static_cast<uint32_t>(vikingRoomMesh->GetIndicesCount()), 1, 0, 0, 0);
//
    //    commandList->EndRendering();
//
    //    commandList->TransitionImageLayout(presentationImages[imageIndex], Core::Layout::Present);
//
    //    commandList->Close();
//
    //    uint64_t signalValue = frameCounter + 1;
//
    //    Core::SubmitInfo submitInfo;
    //    submitInfo.waitSemaphores = { imageAvailablesSemaphores[syncIndex] };
    //    submitInfo.waitSemaphoresValues = { 0 };
    //    submitInfo.signalSemaphores = { frameTimelineSemaphore, renderFinishedSemaphores[imageIndex] };
    //    submitInfo.signalSemaphoresValues = { signalValue, 0 };
    //    submitInfo.stage = Core::PipelineStage::ColorOutput;
//
    //    device->SubmitCommandList(commandList, submitInfo);
//
    //    Core::PresentInfo presentInfo;
    //    presentInfo.waitSemaphores = { renderFinishedSemaphores[imageIndex] };
    //    presentInfo.swapchain = swapchain;
    //    presentInfo.imageIndex = imageIndex;
//
    //    bool sucess = device->Present(presentInfo);
//
    //    if (!sucess)
    //    {
    //        gpuResizeRequest = true;
    //    }
    //    frameCounter++;
    //}
    //device->WaitIdle();
//
    //device->DestroySampler(sampler);
    //device->DestroyBuffer(uniformCamera);
    //device->DestroyBuffer(uniformModel);
//
    //device->DestroyDescriptorSet(frameDataDescriptorSet);
    //device->DestroyDescriptorSet(drawDataDescriptorSet);
//
    //device->DestroyPipeline(unlitPipeline);
    //device->DestroyPipelineLayout(globalPipelineLayout);
    //device->DestroyDescriptorSetsLayouts(globalLayoutDescriptors);
//
    //device->DestroyShader(unlitFrag);
    //device->DestroyShader(baseVert);
    //device->DestroyShader(globalLayoutShader);
//
    //for (uint32_t i = 0; i < swapchain->GetImageCount(); ++i)
    //{
    //    // delete fence & semaphore
    //    device->DestroySemaphore(imageAvailablesSemaphores[i]);
    //    device->DestroySemaphore(renderFinishedSemaphores[i]);
    //}
    //device->DestroySemaphore(frameTimelineSemaphore);
//
    //device->DestroyPresentationImages(presentationImages);
//
    //device->DestroyImage(depthImage);
//
    //device->DestroySwapchain(swapchain);
//
    //Core::KEngine::Clean();
    //Core::KEngine::Shutdown();
//
#ifdef KDEBUG
    do
    {
        std::cout << '\n' << "Press enter to continue...";
    } while (std::cin.get() != '\n');
#endif
    return 0;
}
