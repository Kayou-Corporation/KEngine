#pragma once

#include <string>
#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#include "Core/RHI/Public/RHI.hpp"
#include "Window/WindowRenderer.hpp"

// -------- Base window ----------

struct WindowSpecs
{
	uint32_t width = 1280;
	uint32_t height = 720; 
	std::string name = "Window";

	bool allowResize = true;
	RendererAPI rendererAPI = RendererAPI::Vulkan;
};

class Window : virtual public IResource
{
public:
	virtual ~Window() = default;

	KENGINE_API virtual void Create(const WindowSpecs& specs) = 0;
	KENGINE_API virtual void Destroy() = 0;

	KENGINE_API virtual bool ShouldClose() = 0;
	KENGINE_API virtual void PollEvents() = 0;

	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }
	std::string GetName() const { return m_windowName; }

	RefCountPtr<WindowRenderer> GetWindowRenderer() const { return m_windowRenderer; }

protected:
	uint32_t m_width;
	uint32_t m_height;

	std::string m_windowName;

	RefCountPtr<WindowRenderer> m_windowRenderer;
};

// -------- Interface ----------
enum WindowAPI
{
	SDL = 0
};

struct WindowInterface
{
	static RefCountPtr<Window> InitWindow(WindowAPI api);
};