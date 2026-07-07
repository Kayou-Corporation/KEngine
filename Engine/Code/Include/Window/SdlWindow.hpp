#pragma once

#include <SDL3/SDL.h>

#include "Window.hpp"
#include "WindowRenderer.hpp"
#include "SDL3/SDL_vulkan.h"

BEGIN_NAMESPACE_WINDOW
	class SDLWindow : public Window
{
public:
	SDLWindow() = default;
	virtual ~SDLWindow() override = default;

	virtual void Create(const WindowSpecs& specs) override;
	virtual void Destroy() override;

	virtual bool ShouldClose() override { return !m_isRunning; }
	virtual void PollEvents() override;

	SDL_Window* GetSDLWindow() const { return m_window; }

private:
	SDL_Window* m_window = nullptr;
	bool m_isRunning = false;
};

#ifdef VULKAN_ENABLE
class SDLVulkanWindowRenderer : public VulkanWindowRenderer
{
public:
	SDLVulkanWindowRenderer(Window* ownerWindow);
	virtual ~SDLVulkanWindowRenderer() override = default;

	virtual std::vector<const char*> GetVulkanInstanceExtensions() override;
	virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) override;
private:

};
#endif

END_NAMESPACE_WINDOW