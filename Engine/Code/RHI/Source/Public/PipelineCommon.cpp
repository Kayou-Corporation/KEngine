#include "Public/PipelineCommon.hpp"

BEGIN_NAMESPACE_RHI

Core::RefCountPtr<DescriptorSetLayout> PipelineLayout::GetDescriptorSetLayout(std::string name) const
{
	for (const Core::RefCountPtr<DescriptorSetLayout>& DescriptorSetLayout : m_descriptorsLayouts)
	{
		if (DescriptorSetLayout->GetName() == name)
		{
			return DescriptorSetLayout;
		}
	}

	spdlog::warn("DescriptorSetLayout {} doesn't exist in this pipeline layout", name);

	Core::RefCountPtr<DescriptorSetLayout> EmptyDescriptorSetLayout = Core::CreateRefPtr<DescriptorSetLayout>();
	return EmptyDescriptorSetLayout;
}

Core::RefCountPtr<DescriptorSetLayout> PipelineLayout::GetDescriptorSetLayout(uint32_t index) const
{
	for (const Core::RefCountPtr<DescriptorSetLayout>& DescriptorSetLayout : m_descriptorsLayouts)
	{
		if (DescriptorSetLayout->GetIndex() == index)
		{
			return DescriptorSetLayout;
		}
	}

	spdlog::warn("DescriptorSetLayout index {} doesn't exist in this pipeline layout", index);

	Core::RefCountPtr<DescriptorSetLayout> EmptyDescriptorSetLayout = Core::CreateRefPtr<DescriptorSetLayout>();
	return EmptyDescriptorSetLayout;
}

std::vector<std::string> PipelineLayout::GetAllDescriptorSetsLayoutsNames() const
{
	std::vector<std::string> allDescriptorSetsLayoutsNames;
	for (const Core::RefCountPtr<DescriptorSetLayout>& DescriptorSetLayout : m_descriptorsLayouts)
	{
		allDescriptorSetsLayoutsNames.push_back(DescriptorSetLayout->GetName());
	}

	return allDescriptorSetsLayoutsNames;
}

uint32_t PipelineLayout::GetDescriptorSetLayoutIndex(std::string name) const
{
	for (const Core::RefCountPtr<DescriptorSetLayout>& DescriptorSetLayout : m_descriptorsLayouts)
	{
		if (DescriptorSetLayout->GetName() == name)
		{
			return DescriptorSetLayout->GetIndex();
		}
	}

	spdlog::warn("DescriptorSetLayout {} doesn't exist in this pipeline layout", name);
	return 0;
}

Core::RefCountPtr<PushConstantLayout> PipelineLayout::GetPushConstantLayouts(std::string name) const
{
	for (const Core::RefCountPtr<PushConstantLayout>& PushConstantLayout : m_pushConstantsLayouts)
	{
		if (PushConstantLayout->GetName() == name)
		{
			return PushConstantLayout;
		}
	}

	spdlog::warn("PushConstantLayout {} doesn't exist in this pipeline layout", name);

	Core::RefCountPtr<PushConstantLayout> EmptyPushConstantLayout = Core::CreateRefPtr<PushConstantLayout>();
	return EmptyPushConstantLayout;
}

END_NAMESPACE_RHI