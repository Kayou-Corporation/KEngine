#pragma once

#include <SDL3/SDL.h>

#include "Window/Window.hpp"

class SDLWindow : public RefCounter<Window>
{
public:
	void Create(const WindowSpecs& specs) override;
	void Destroy() override;

	bool ShouldClose() override { return !m_isRunning; }
	void PollEvents() override;

	//std::vector<const char*> GetVulkanInstanceExtension() override;


private:
	SDL_Window* m_window;
	bool m_isRunning = false;
};

#ifdef VULKAN_ENABLE
class SDLVulkanWindowRenderer : public VulkanWindowRenderer
{
public:
	SDLVulkanWindowRenderer() = default;
	~SDLVulkanWindowRenderer() = default;

	std::vector<const char*> GetVulkanInstanceExtensions() override;
	VkSurfaceKHR CreateVulkanSurface(VkInstance instance) override;
};
#endif
