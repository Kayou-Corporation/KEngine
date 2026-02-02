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

	std::vector<const char*> GetVulkanInstanceExtension() override;


private:
	SDL_Window* m_window;
	bool m_isRunning = false;
};