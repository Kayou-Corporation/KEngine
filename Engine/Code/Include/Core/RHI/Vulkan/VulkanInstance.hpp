#pragma once

#include "RHI/API/Instance.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_CORE

class VulkanInstance : public Instance
{
public:
	VulkanInstance() = default;
	virtual ~VulkanInstance() override = default;

	virtual void Create(const InstanceSpecs& specs) override;
	virtual void Destroy() override;

	virtual SurfaceHandle CreateSurface(const SurfaceSpecs& specs) override;
	virtual void DestroySurface(SurfaceHandle surface) override;

	virtual DeviceHandle CreateDevice(const DeviceSpecs& specs) override;
	virtual void DestroyDevice(DeviceHandle device) override;

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

END_NAMESPACE_CORE