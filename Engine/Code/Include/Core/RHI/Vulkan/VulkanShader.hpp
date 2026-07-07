#pragma once

#include "RHI/API/Shader.hpp"

#include "RHI/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanShader : public Shader
{
public:
	VulkanShader() = default;
	virtual ~VulkanShader() override = default;

public:
	const vk::ShaderModule& GetModule() const { return m_module; }

	void SetModule(const vk::ShaderModule& module) { m_module = module; }

private:
	vk::ShaderModule m_module;
};

END_NAMESPACE_RHI