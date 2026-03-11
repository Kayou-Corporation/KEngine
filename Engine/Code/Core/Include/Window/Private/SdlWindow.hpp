#pragma once

#include <SDL3/SDL.h>

#include "Window/Window.hpp"

BEGIN_NAMESPACE_CORE

class SDLWindow : public Window
{
public:
	SDLWindow() = default;
	virtual ~SDLWindow() override = default;

	void Create(const WindowSpecs& specs) override;
	void Destroy() override;

	bool ShouldClose() override { return !m_isRunning; }
	void PollEvents() override;

	SDL_Window* GetSDLWindow() const { return m_window; }

private:
	SDL_Window* m_window = nullptr;
	bool m_isRunning = false;
};

//#ifdef VULKAN_ENABLE
class SDLVulkanWindowRenderer : public VulkanWindowRenderer
{
public:
	SDLVulkanWindowRenderer(Window* ownerWindow);
	virtual ~SDLVulkanWindowRenderer() override = default;

	std::vector<const char*> GetVulkanInstanceExtensions() override;
	VkSurfaceKHR CreateVulkanSurface(VkInstance instance) override;
private:

};
//#endif

END_NAMESPACE_CORE