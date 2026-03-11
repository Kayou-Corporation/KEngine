#pragma once

#include "Public/Instance.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanInstance : public Instance
{
public:
	VulkanInstance() = default;
	virtual ~VulkanInstance() override = default;

	void Create(const InstanceSpecs& specs) override;
	void Destroy() override;

	Core::RefCountPtr<Surface> CreateSurface(const SurfaceSpecs& specs) override;
	void DestroySurface(Core::RefCountPtr<Surface> surface) override;

	Core::RefCountPtr<Device> CreateDevice(const DeviceSpecs& specs) override;
	void DestroyDevice(Core::RefCountPtr<Device> device) override;

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

END_NAMESPACE_RHI