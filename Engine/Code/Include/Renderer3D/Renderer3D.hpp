#pragma once

#include "Renderer3DModule.hpp"

#include "RHI/API/Swapchain.hpp"
#include "RHI/API/DescriptorPool.hpp"
#include "RHI/API/DescriptorSetArray.hpp"
#include "RHI/API/DescriptorSet.hpp"
#include <vector>

#include "GraphicPass.hpp"

BEGIN_NAMESPACE_3DRENDERER

#define GLOBALDESCRIPTOR_FRAME_NAME "frameData"
#define GLOBALDESCRIPTOR_DRAW_NAME "drawData"

#define GLOBALLAYOUT_NAME "globalLayout"
#define GLOBALLAYOUT_TEXTURES_NAME "globalTextures"
#define GLOBALLAYOUT_MODELS_NAME "globalModels"

enum DrawCommandFlag : uint32_t
{
    ColorPass = 0,
    ShadowPass = 1 << 0,
    LightingPass = 1 << 1
};

enum DrawCommandType
{
    Opaque,
    Transparent
};

enum RenderingType
{
    Unlit,
    Lit
};

struct DrawCommand
{
    // Which pass it's draw on
    DrawCommandFlag passFlags;
    DrawCommandType type;
    RenderingType renderingType;

    uint32_t modelID;
    uint32_t textureID;
    uint32_t samplerID;
};

class Renderer3D
{
public:
    Renderer3D() = delete;
    virtual ~Renderer3D();

    // !!!! Must implement this in override
    virtual void SetupRenderer() = 0;
    virtual void CleanupRenderer() = 0;

    // Public interface for the renderer
    void PushDrawCommand(const DrawCommand& command) {m_unResolvedDrawCommands.push_back(command);}
    void Draw();


protected:
    // Base object creation
    Renderer3D(Core::SwapchainHandle targetSwapchain);

    // Create / Destroy syncronisation
    void CreateSyncronisationObjects();
    void DestroySyncronisationObjects();

    // Create / Destroy layouts
    void CreateGlobalLayouts();
    void DestroyGlobalLayouts();

     // Create / Destroy Descriptors
    void CreateGlobalRessourcesDescriptors();
    void DestroyGlobalRessourcesDescriptors();
    void CreateGlobalRessourcesDescriptorType(const std::vector<Core::DescriptorSetLayoutHandle>& layouts, bool array);

    void ExecuteGraphicPass(GraphicPass& pass, uint32_t imageIndex);
    // !!!! Must implement this in override
    virtual void ResolveRessources() = 0;
protected:
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

    // Graphic passes
    std::vector<GraphicPass> m_graphicPasses;

    // Unresolved resources
    std::vector<DrawCommand> m_unResolvedDrawCommands;
};

END_NAMESPACE_3DRENDERER
