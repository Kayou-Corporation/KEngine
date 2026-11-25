#pragma once

#include "Renderer3D.hpp"

BEGIN_NAMESPACE_3DRENDERER

class ForwardRenderer3D : public Renderer3D
{
public:
    ForwardRenderer3D() = delete;
    ForwardRenderer3D(Core::SwapchainHandle targetSwapchain);
    ~ForwardRenderer3D() override;

    void SetupRenderer() override;
    void CleanupRenderer() override;

private:
    void ResolveRessources() override;

    void CreateBasePass();
    void DestroyBasePass();

};

END_NAMESPACE_3DRENDERER