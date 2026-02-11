#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#include "Core/RHI/Public/Instance.hpp"

class VulkanInstance : public Instance
{
public:
	VulkanInstance() = default;
	virtual ~VulkanInstance() override = default;

	void Create(const InstanceSpecs& specs) override;
	void Destroy() override;

	RefCountPtr<Surface> CreateSurface(const SurfaceSpecs& specs) override;
	void DestroySurface(RefCountPtr<Surface> surface) override;

	RefCountPtr<Device> CreateDevice(const DeviceSpecs& specs) override;
	void DestroyDevice(RefCountPtr<Device> device) override;

protected:
	Version m_engineVersion;

private:
	std::vector<const char*> CheckValidationLayersSupport(const std::vector<const char*>& requestedLayers);

	static VkBool32 DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT messageType,
		const vk::DebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);

	vk::Instance m_handle;
	vk::DebugUtilsMessengerEXT m_debugHandler{};
	std::vector<const char*> m_debugLayers;

	vk::detail::DispatchLoaderDynamic m_dispatchLoader;
};