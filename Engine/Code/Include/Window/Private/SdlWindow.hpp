#pragma once

#include <SDL3/SDL.h>

#include "Window/Window.hpp"

class SDLWindow : public Window
{
public:
	SDLWindow() = default;
	virtual ~SDLWindow() override = default;

	void Create(const WindowSpecs& specs) override;
	void Destroy() override;

	bool ShouldClose() override { return !m_isRunning; }
	void PollEvents() override;

private:
	SDL_Window* m_window;
	bool m_isRunning = false;
};

#ifdef VULKAN_ENABLE
class SDLVulkanWindowRenderer : public VulkanWindowRenderer
{
public:
	SDLVulkanWindowRenderer() = default;
	virtual ~SDLVulkanWindowRenderer() override = default;

	std::vector<const char*> GetVulkanInstanceExtensions() override;
	VkSurfaceKHR CreateVulkanSurface(VkInstance instance) override;
};
#endif
