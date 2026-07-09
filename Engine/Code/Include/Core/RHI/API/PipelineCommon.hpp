#pragma once

#include "RHI.hpp"

BEGIN_NAMESPACE_CORE

class DescriptorSetLayout : virtual public Core::IResource
{
public:
	virtual ~DescriptorSetLayout() = default;

	KAPI std::string GetName() const { return m_descriptorName; }
	KAPI uint32_t GetIndex() const { return m_descriptorIndex; }

	KAPI void SetName(std::string name) { m_descriptorName = name; }
	KAPI void SetIndex(uint32_t index) { m_descriptorIndex = index; }

protected:
	std::string m_descriptorName;
	uint32_t m_descriptorIndex;
};


class PushConstantLayout : virtual public Core::IResource
{
public:
	virtual ~PushConstantLayout() = default;

	KAPI std::string GetName() const { return m_name; }
	KAPI uint32_t GetSize() const { return m_totalSize; }
	KAPI ShaderStage GetStage() const { return m_stage; }

	KAPI void SetName(std::string name) { m_name = name; }
	KAPI void SetSize(uint32_t offset) { m_totalSize = offset; }
	KAPI void SetStage(ShaderStage stage) { m_stage = stage; }

protected:
	std::string  m_name;
	ShaderStage m_stage;
	uint32_t m_totalSize;
};

class PipelineLayout : virtual public Core::IResource
{
public:
	virtual ~PipelineLayout() = default;

	KAPI DescriptorSetLayoutHandle GetDescriptorSetLayout(std::string name) const;
	KAPI DescriptorSetLayoutHandle GetDescriptorSetLayout(uint32_t index) const;
	KAPI std::vector<DescriptorSetLayoutHandle>& GetAllDescriptorSetsLayouts() { return m_descriptorsLayouts; }
	KAPI std::vector<std::string> GetAllDescriptorSetsLayoutsNames()const;
	KAPI uint32_t GetDescriptorSetLayoutIndex(std::string name) const;

	KAPI PushConstantLayoutHandle GetPushConstantLayouts(std::string name) const;
	KAPI std::vector<PushConstantLayoutHandle>& GetAllPushConstantsLayouts() { return m_pushConstantsLayouts; }

	KAPI void AddDescriptorSetLayout(DescriptorSetLayoutHandle layout) { m_descriptorsLayouts.push_back(layout); }
	KAPI void AddPushConstantLayout(PushConstantLayoutHandle layout) { m_pushConstantsLayouts.push_back(layout); }

protected:
	std::vector<DescriptorSetLayoutHandle> m_descriptorsLayouts;
	std::vector<PushConstantLayoutHandle> m_pushConstantsLayouts;
};

class Pipeline : virtual public Core::IResource
{
public:
	virtual ~Pipeline() = default;

	KAPI PipelineType GetType() { return m_type; }
	KAPI PipelineLayoutHandle GetLayout() { return m_layout; }

	KAPI void SetType(PipelineType type) { m_type = type; }
	KAPI void SetLayout(PipelineLayoutHandle layout) { m_layout = layout; }


protected:
	PipelineType m_type;
	PipelineLayoutHandle m_layout;
};



END_NAMESPACE_CORE