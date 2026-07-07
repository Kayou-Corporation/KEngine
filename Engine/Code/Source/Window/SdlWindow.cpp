#include "SdlWindow.hpp"

#include <SDL3/SDL_vulkan.h>

#ifndef VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#define VK_EXT_DEBUG_REPORT_EXTENSION_NAME "VK_EXT_debug_report"
#endif

#include "spdlog/spdlog.h"

BEGIN_NAMESPACE_WINDOW

void SDLWindow::Create(const WindowSpecs& specs)
{
	m_width = specs.width;
	m_height = specs.height;
	m_windowName = specs.name;

	SDL_Init(SDL_INIT_VIDEO);

	switch (specs.rendererAPI)
	{
	case Core::RendererAPI::Vulkan:
		m_window = SDL_CreateWindow(m_windowName.c_str(), static_cast<int>(m_width), static_cast<int>(m_height), SDL_WINDOW_VULKAN);

		Window* myWindowPtr = static_cast<Window*>(this);
		m_windowRenderer = Core::CreateRefPtr<SDLVulkanWindowRenderer>(myWindowPtr);
		break;
	}
	
	if (m_window == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
	}

	SDL_SetWindowResizable(m_window, specs.allowResize);

	m_isRunning = true;
}

void SDLWindow::Destroy()
{
	m_windowRenderer = nullptr;

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
		else if (event.type == SDL_EVENT_WINDOW_RESIZED)
		{
			m_width = event.window.data1;
			m_height = event.window.data2;

			m_HasResize = true;
			break;
		}
	}
}

#ifdef VULKAN_ENABLE

std::vector<const char*> SDLVulkanWindowRenderer::GetVulkanInstanceExtensions()
{
	uint32_t sdlInstanceExtensionsCount = 0;
	const char* const* sdlInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlInstanceExtensionsCount);

	if (sdlInstanceExtensions == nullptr)
	{ 
		spdlog::critical("Can't get vulkan extensions from window");
	}

	std::vector<const char*> extensions;
	if (sdlInstanceExtensions && sdlInstanceExtensionsCount > 0)
		extensions.assign(sdlInstanceExtensions, sdlInstanceExtensions + sdlInstanceExtensionsCount);

#ifdef KENGINE_DEBUG
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
	return extensions;
}

VkSurfaceKHR SDLVulkanWindowRenderer::CreateVulkanSurface(VkInstance instance)
{
	VkSurfaceKHR surface;

	SDLWindow* sdlWin = static_cast<SDLWindow*>(m_ownerWindow);

	if (!sdlWin)
	{
		spdlog::error("Can't cast window into SDlWindow");
		return nullptr;	
	}
	SDL_Vulkan_CreateSurface(sdlWin->GetSDLWindow(), instance, nullptr, &surface);

	return surface;
}

#endif

SDLVulkanWindowRenderer::SDLVulkanWindowRenderer(Window* ownerWindow)
{
	m_ownerWindow = ownerWindow;
}

END_NAMESPACE_WINDOW