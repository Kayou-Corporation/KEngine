#include "Window/Private/SdlWindow.hpp"

#include <SDL3/SDL_vulkan.h>

#ifndef VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#define VK_EXT_DEBUG_REPORT_EXTENSION_NAME "VK_EXT_debug_report"
#endif

#include "spdlog/spdlog.h"

void SDLWindow::Create(const WindowSpecs& specs)
{
	m_width = specs.width;
	m_height = specs.height;
	m_windowName = specs.name;

	SDL_Init(SDL_INIT_VIDEO);

	switch (specs.rendererAPI)
	{
	case RendererAPI::Vulkan:
		m_window = SDL_CreateWindow(m_windowName.c_str(), m_width, m_height, SDL_WINDOW_VULKAN);
		m_windowRenderer = new SDLVulkanWindowRenderer;
		break;
	}
	
	if (m_window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
	}

	SDL_SetWindowResizable(m_window, specs.allowResize);

	m_isRunning = true;
}

void SDLWindow::Destroy()
{
	SDL_DestroyWindow(m_window);

	SDL_Quit();
}

void SDLWindow::PollEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
		{
			m_isRunning = false;
		}
	}
}

#ifdef VULKAN_ENABLE

std::vector<const char*> SDLVulkanWindowRenderer::GetVulkanInstanceExtensions()
{
	uint32_t sdlInstanceExtensionsCount = 0;
	const char* const* sdlInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlInstanceExtensionsCount);

	if (sdlInstanceExtensions == NULL) 
	{ 
		spdlog::critical("Can't get vulkan extensions from window");
	}

	std::vector<const char*> extensions(sdlInstanceExtensions, sdlInstanceExtensions + sdlInstanceExtensionsCount);

#ifdef KENGINE_DEBUG
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
	return extensions;
}

VkSurfaceKHR SDLVulkanWindowRenderer::CreateVulkanSurface(VkInstance instance)
{
	(void)instance;

	//VkSurfaceKHR vkSurface;

	return nullptr;
}

#endif