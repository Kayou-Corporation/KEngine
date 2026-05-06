#include "Private/Vulkan/VulkanDescriptorSet.hpp"

#include "Private/Vulkan/VulkanBuffer.hpp"
#include "Private/Vulkan/VulkanImage.hpp"
#include "Private/Vulkan/VulkanTranslate.hpp"

BEGIN_NAMESPACE_RHI

void VulkanDescriptorSet::SetBuffer(uint32_t index, DescriptorType type, Core::RefCountPtr<Buffer> buffer)
{
	for (const VulkanBinding& vulkanBinding : m_bindings)
	{
		if (vulkanBinding.binding.binding == index && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer.CastAs<VulkanBuffer>()->GetHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = VK_WHOLE_SIZE;

			vk::WriteDescriptorSet write{};
			write.dstSet = m_handle;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfo;

			break;
		}
	}
}

void VulkanDescriptorSet::SetBuffer(std::string name, DescriptorType type, Core::RefCountPtr<Buffer> buffer)
{
	for (const VulkanBinding& vulkanBinding : m_bindings)
	{
		if (vulkanBinding.name == name && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer.CastAs<VulkanBuffer>()->GetHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = VK_WHOLE_SIZE;

			vk::WriteDescriptorSet write{};
			write.dstSet = m_handle;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfo;

			break;
		}
	}
}

void VulkanDescriptorSet::SetImage(std::string name, DescriptorType type, Core::RefCountPtr<Image> RHIImage)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIImage.CastAs<VulkanImage>();

	for (const VulkanBinding& vulkanBinding : m_bindings)
	{
		if (vulkanBinding.name == name && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			//vk::DescriptorImageInfo imageInfo{};
			//imageInfo.imageView = RHIVulkanImage->GetHandleView();
			//imageInfo.imageLayout = RHIVulkanImage->GetLayout();
			//imageInfo.range = VK_WHOLE_SIZE;

			vk::WriteDescriptorSet write{};
			write.dstSet = m_handle;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			//write.pImageInfo = &imageInfo;

			break;
		}
	}
}

void VulkanDescriptorSet::SetImage(uint32_t index, DescriptorType type, Core::RefCountPtr<Image> RHIImage)
{
	Core::RefCountPtr<VulkanImage> RHIVulkanImage = RHIImage.CastAs<VulkanImage>();

	for (const VulkanBinding& vulkanBinding : m_bindings)
	{
		if (vulkanBinding.binding.binding == index && vulkanBinding.binding.descriptorType == TranslateToVulkan(type))
		{
			//vk::DescriptorImageInfo imageInfo{};
			//imageInfo.imageView = RHIVulkanImage->GetHandleView();
			//imageInfo.imageLayout = RHIVulkanImage->GetLayout();
			//imageInfo.range = VK_WHOLE_SIZE;

			vk::WriteDescriptorSet write{};
			write.dstSet = m_handle;
			write.dstBinding = vulkanBinding.binding.binding;
			write.dstArrayElement = 0;
			write.descriptorType = vulkanBinding.binding.descriptorType;
			write.descriptorCount = 1;
			//write.pImageInfo = &imageInfo;

			break;
		}
	}
}

vk::DescriptorSetAllocateInfo VulkanDescriptorSet::GetAllocInfo(vk::DescriptorSetLayout layout)
{
	vk::DescriptorSetAllocateInfo alloInfo{};
	alloInfo.descriptorPool = m_pool;
	alloInfo.descriptorSetCount = 1;
	alloInfo.pSetLayouts = &layout;
	
	return alloInfo;
}

vk::DescriptorPoolCreateInfo VulkanDescriptorSet::GetPoolCreateInfo(std::map<vk::DescriptorType, uint32_t> descriptors, uint32_t totalCount)
{
	vk::DescriptorPoolCreateInfo createInfo{};

	for (const auto& [type, count] : descriptors)
	{
		vk::DescriptorPoolSize typeSize;
		typeSize.type = type;
		typeSize.descriptorCount = count;
		m_poolSizes.push_back(typeSize);
	}

	createInfo.poolSizeCount = m_poolSizes.size();
	createInfo.pPoolSizes = m_poolSizes.data();
	createInfo.maxSets = totalCount;

	return createInfo;
}

END_NAMESPACE_RHI