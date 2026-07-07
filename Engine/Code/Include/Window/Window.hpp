#pragma once

#include <string>
#include <vector>

#include "Utils/Core.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Export.hpp"

#define WINDOW_NAMESPACE Window

#define BEGIN_NAMESPACE_WINDOW BEGIN_NAMESPACE_KAYOU namespace WINDOW_NAMESPACE {
#define END_NAMESPACE_WINDOW } /* namespace WINDOW_NAMESPACE */ END_NAMESPACE_KAYOU

BEGIN_NAMESPACE_WINDOW

class WindowRenderer;

// -------- Base window ----------

struct WindowSpecs
{
	uint32_t width = 1280;
	uint32_t height = 720; 
	std::string name = "Window";

	bool allowResize = true;
	Core::RendererAPI rendererAPI = Core::RendererAPI::Vulkan;
};

class Window : virtual public Core::IResource
{
public:
	virtual ~Window() = default;

	KENGINE_API virtual void Create(const WindowSpecs& specs) = 0;
	KENGINE_API virtual void Destroy() = 0;

	KENGINE_API virtual bool ShouldClose() = 0;
	KENGINE_API virtual void PollEvents() = 0;

	void ResizeComplete() { m_HasResize = false; }

	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }
	std::string GetName() const { return m_windowName; }
	bool GetHasResize() const { return m_HasResize; }

	Core::RefCountPtr<WindowRenderer> GetWindowRenderer() const { return m_windowRenderer; }

protected:
	uint32_t m_width = 0u;
	uint32_t m_height = 0u;

	bool m_HasResize = false;

	std::string m_windowName;

	Core::RefCountPtr<WindowRenderer> m_windowRenderer;
};

// -------- Interface ----------
enum WindowAPI
{
	SDL = 0
};

struct WindowInterface
{
	static Core::RefCountPtr<Window> InitWindow(WindowAPI api);
};

END_NAMESPACE_WINDOW