#include "RHI/API/PipelineCommon.hpp"

BEGIN_NAMESPACE_RHI

DescriptorSetLayoutHandle PipelineLayout::GetDescriptorSetLayout(std::string name) const
{
	for (const DescriptorSetLayoutHandle& DescriptorSetLayout : m_descriptorsLayouts)
	{
		if (DescriptorSetLayout->GetName() == name)
		{
			return DescriptorSetLayout;
		}
	}

	spdlog::warn("DescriptorSetLayout {} doesn't exist in this pipeline layout", name);

	DescriptorSetLayoutHandle EmptyDescriptorSetLayout = Core::CreateRefPtr<DescriptorSetLayout>();
	return EmptyDescriptorSetLayout;
}

DescriptorSetLayoutHandle PipelineLayout::GetDescriptorSetLayout(uint32_t index) const
{
	for (const DescriptorSetLayoutHandle& DescriptorSetLayout : m_descriptorsLayouts)
	{
		if (DescriptorSetLayout->GetIndex() == index)
		{
			return DescriptorSetLayout;
		}
	}

	spdlog::warn("DescriptorSetLayout index {} doesn't exist in this pipeline layout", index);

	DescriptorSetLayoutHandle EmptyDescriptorSetLayout = Core::CreateRefPtr<DescriptorSetLayout>();
	return EmptyDescriptorSetLayout;
}

std::vector<std::string> PipelineLayout::GetAllDescriptorSetsLayoutsNames() const
{
	std::vector<std::string> allDescriptorSetsLayoutsNames;
	for (const DescriptorSetLayoutHandle& DescriptorSetLayout : m_descriptorsLayouts)
	{
		allDescriptorSetsLayoutsNames.push_back(DescriptorSetLayout->GetName());
	}

	return allDescriptorSetsLayoutsNames;
}

uint32_t PipelineLayout::GetDescriptorSetLayoutIndex(std::string name) const
{
	for (const DescriptorSetLayoutHandle& DescriptorSetLayout : m_descriptorsLayouts)
	{
		if (DescriptorSetLayout->GetName() == name)
		{
			return DescriptorSetLayout->GetIndex();
		}
	}

	spdlog::warn("DescriptorSetLayout {} doesn't exist in this pipeline layout", name);
	return 0;
}

PushConstantLayoutHandle PipelineLayout::GetPushConstantLayouts(std::string name) const
{
	for (const PushConstantLayoutHandle& PushConstantLayout : m_pushConstantsLayouts)
	{
		if (PushConstantLayout->GetName() == name)
		{
			return PushConstantLayout;
		}
	}

	spdlog::warn("PushConstantLayout {} doesn't exist in this pipeline layout", name);

	PushConstantLayoutHandle EmptyPushConstantLayout = Core::CreateRefPtr<PushConstantLayout>();
	return EmptyPushConstantLayout;
}

END_NAMESPACE_RHI