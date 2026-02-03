#pragma once

#include <vulkan/vulkan.hpp>

#include "Core/RHI/Public/Instance.hpp"

class VulkanInstance : public RefCounter<Instance>
{
public:
	~VulkanInstance() override = default;

	void Create(const InstanceSpecs& specs) override;
	void Destroy() override;

	//KENGINE_API RefCountPtr<Surface> CreateSurface(RefCountPtr<Window> window) override;
	//KENGINE_API void DestroySurface(RefCountPtr<Surface>) override;

protected:
	Version m_engineVersion;

private:
	vk::Instance m_handle;
};