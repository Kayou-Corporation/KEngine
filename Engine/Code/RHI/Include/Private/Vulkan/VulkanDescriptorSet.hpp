#pragma once

#include "Public/DescriptorSet.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanPipelineCommon.hpp"

#include <map>

BEGIN_NAMESPACE_RHI

class VulkanDescriptorSet : public DescriptorSet
{
public:
	VulkanDescriptorSet() = default;
	virtual ~VulkanDescriptorSet() override = default;

	virtual void SetBuffer(uint32_t index, DescriptorType type, Core::RefCountPtr<Buffer> buffer) override;
	virtual void SetBuffer(std::string name, DescriptorType type, Core::RefCountPtr<Buffer> buffer) override;

	virtual void SetImage(std::string name, DescriptorType type, Core::RefCountPtr<Image> image) override;
	virtual void SetImage(uint32_t index, DescriptorType type, Core::RefCountPtr<Image> image) override;

public:
	vk::DescriptorSetAllocateInfo GetAllocInfo(vk::DescriptorSetLayout layout);
	vk::DescriptorPoolCreateInfo GetPoolCreateInfo(std::map<vk::DescriptorType, uint32_t> descriptors, uint32_t totalCount);

	void SetHanlde(vk::DescriptorSet descriptorSet) { m_handle = descriptorSet; }
	void SetPool(vk::DescriptorPool pool) { m_pool = pool; }
	void SetBindings(std::vector<VulkanBinding>& bindings) { m_bindings = bindings; }

	vk::DescriptorSet GetHandle() { return m_handle; }
	vk::DescriptorPool GetPool() { return m_pool; }

private:
	vk::DescriptorSet m_handle;
	vk::DescriptorPool m_pool;
	std::vector<vk::DescriptorPoolSize> m_poolSizes;

	std::vector<VulkanBinding> m_bindings;
};

END_NAMESPACE_RHI