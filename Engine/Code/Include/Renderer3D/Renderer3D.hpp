#pragma once

#include "Renderer3DModule.hpp"

#include "RHI/API/Swapchain.hpp"

#include <vector>

#include "GraphicPass.hpp"

BEGIN_NAMESPACE_3DRENDERER
#define GLOBALDESCRIPTOR_FRAME_NAME "frameData"
#define GLOBALDESCRIPTOR_DRAW_NAME "drawData"

//#define FRAMEDESCRIPOR_ATTRIBUTE_CAMERA_NAME "camera"

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
    // Target & info
    Core::SwapchainHandle m_targetSwapchain;
    std::vector<Core::ImageHandle> m_renderTargetImages;
    Core::ImageHandle m_renderTargerDepthImages;
    uint64_t m_frameInFlight;

    // Syncro
    uint64_t m_frameCounter = 0;
    std::vector<Core::SemaphoreHandle> m_renderTargetAvailableSemaphores;
    std::vector<Core::SemaphoreHandle> m_renderFinishedSemaphores;
    Core::SemaphoreHandle m_frameTimelineSemaphore;

    // Pass
    std::vector<GraphicPass> m_graphicPasses;
    //std::pair<std::string>


    // Global resources ( for now just camera position )
    // Shader source
    Core::ShaderHandle m_globalShader;

    // Global pipeline layout (from global shader)
    Core::PipelineLayoutHandle m_globalPipelineLayout;

    // Global shader layout (bindings)
    std::vector<Core::DescriptorSetLayoutHandle> m_globalDescriptorSetsLayouts;

    // Global descriptor set (from global pipeline layout)
    std::vector<Core::DescriptorSetHandle> m_globalDescriptorSets;

    // Shader library
    std::vector<Core::ShaderHandle> m_shadersLibrary;

};

END_NAMESPACE_3DRENDERER
