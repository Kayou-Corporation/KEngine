#pragma once

#include <string>
#include <vector>

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

#include "Core/RHI/Public/RHI.hpp"

#ifdef VULKAN_ENABLE
#include <vulkan/vulkan.h>
#endif

struct WindowSpecs
{
	uint32_t width = 1280;
	uint32_t height = 720; 
	std::string name = "Window";

	bool allowResize = true;
	RendererAPI rendererAPI = RendererAPI::Vulkan;
};

class Window : public IResource
{
public:
	~Window() = default;

	KENGINE_API virtual void Create(const WindowSpecs& specs) = 0;
	KENGINE_API virtual void Destroy() = 0;

	KENGINE_API virtual bool ShouldClose() = 0;
	KENGINE_API virtual void PollEvents() = 0;

	KENGINE_API uint32_t GetWidth() const { return m_width; }
	KENGINE_API uint32_t GetHeight() const { return m_height; }
	KENGINE_API std::string GetName() const { return m_windowName; }

//#ifdef VULKAN_ENABLE
//	KENGINE_API virtual std::vector<const char*> GetVulkanInstanceExtension() = 0;
//	KENGINE_API virtual VkSurfaceKHR CreateVulkanSurface() = 0;
//#endif

protected:
	uint32_t m_width;
	uint32_t m_height;

	std::string m_windowName;

private:
	class Internal
	{
		virtual ~Internal() = default;

#ifdef VULKAN_ENABLE
		virtual std::vector<const char*> GetVulkanInstanceExtensions() const = 0;
		virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;
#endif
	};

};

enum WindowAPI
{
	SDL = 0
};

struct WindowInterface
{
	static RefCountPtr<Window> InitWindow(WindowAPI api);
};