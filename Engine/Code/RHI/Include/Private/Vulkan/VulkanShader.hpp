#pragma once

#include "Public/Shader.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"

BEGIN_NAMESPACE_RHI

class VulkanShader : public Shader
{
public:
	VulkanShader() = default;
	virtual ~VulkanShader() override = default;

public:
	const vk::ShaderModule& GetModule() const { return m_module; }
	ShaderType GetShaderType() const { return m_type; }

	void SetModule(const vk::ShaderModule& module) { m_module = module; }
	void SetShaderType(const ShaderType& type) { m_type = type; }

private:
	vk::ShaderModule m_module;
	ShaderType m_type;
};

END_NAMESPACE_RHI