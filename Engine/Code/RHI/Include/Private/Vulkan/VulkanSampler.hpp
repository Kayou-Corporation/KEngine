#pragma once

#include "Public/Sampler.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanSampler : public Sampler
{
public:
	VulkanSampler() = default;
	virtual ~VulkanSampler() override = default;

	void SetHandle(vk::Sampler sampler) { m_handle = sampler; }
	vk::Sampler GetHandle() { return m_handle; }

	vk::SamplerCreateInfo GetCreateInfo(SamplerSpecs specs, float maxDeviceAnisotropy);

private:
	vk::Sampler m_handle;
};

END_NAMESPACE_RHI