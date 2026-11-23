#pragma once

#include "Renderer3DModule.hpp"

#include "RHI/API/Swapchain.hpp"

#include <vector>

#include "GraphicPass.hpp"

BEGIN_NAMESPACE_3DRENDERER

#define GLOBALDESCRIPTOR_FRAME_NAME "frameData"
#define GLOBALDESCRIPTOR_DRAW_NAME "drawData"

#define GLOBALLAYOUT_NAME "globalLayout"
#define GLOBALLAYOUT_TEXTURES_NAME "globalTextures"
#define GLOBALLAYOUT_MODELS_NAME "globalModels"


class Renderer3D
{
public:
    Renderer3D() = delete;
    Renderer3D(Core::SwapchainHandle targetSwapchain);

    ~Renderer3D();

    void SetupRenderer();
    void Draw();

    void ExecuteGraphicPass(const GraphicPass& pass, uint32_t imageIndex);

private:
    void CreateShaderLibrary();
    void DestroyShaderLibrary();

    void CreateBasePass();
    void DestroyBasePass();

private:
    // Target & info :
    Core::SwapchainHandle m_targetSwapchain;
    std::vector<Core::ImageHandle> m_renderTargetImages;
    Core::ImageHandle m_renderTargetDepthImages;
    uint64_t m_frameInFlight;

    // Syncro :
    uint64_t m_frameCounter = 0;
    std::vector<Core::SemaphoreHandle> m_renderTargetAvailableSemaphores;
    std::vector<Core::SemaphoreHandle> m_renderFinishedSemaphores;
    Core::SemaphoreHandle m_frameTimelineSemaphore;

    // Global ressources Description :
    Core::ShaderHandle m_globalLayoutShader;
    std::vector<Core::DescriptorSetLayoutHandle> m_globalLayoutDescriptorSets;
    Core::PushConstantLayoutHandle m_globalLayoutPushConstants;
    Core::PipelineLayoutHandle m_globalLayout;

    // Global resources binding reference :
    // For boundless descriptor
    std::vector<Core::DescriptorSetArrayHandle> m_globalDescriptorSetsArray;
    std::vector<Core::DescriptorPoolHandle> m_globalDescriptorSetsArrayPool;
    // For classic descriptor
    std::vector<Core::DescriptorSetHandle> m_globalDescriptorSets;
    Core::DescriptorPoolHandle m_globalDescriptorSetsPool;

    // Shader library
    std::vector<Core::ShaderHandle> m_shadersLibrary;

    // Graphic passes



};

END_NAMESPACE_3DRENDERER
