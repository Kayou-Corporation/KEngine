#pragma once

#include "RHI.hpp"

BEGIN_NAMESPACE_CORE

class DescriptorSetLayout : virtual public Core::IResource
{
public:
	virtual ~DescriptorSetLayout() = default;

	KENGINE_API std::string GetName() const { return m_descriptorName; }
	KENGINE_API uint32_t GetIndex() const { return m_descriptorIndex; }

	KENGINE_API void SetName(std::string name) { m_descriptorName = name; }
	KENGINE_API void SetIndex(uint32_t index) { m_descriptorIndex = index; }

protected:
	std::string m_descriptorName;
	uint32_t m_descriptorIndex;
};


class PushConstantLayout : virtual public Core::IResource
{
public:
	virtual ~PushConstantLayout() = default;

	KENGINE_API std::string GetName() const { return m_name; }
	KENGINE_API uint32_t GetSize() const { return m_totalSize; }
	KENGINE_API ShaderStage GetStage() const { return m_stage; }

	KENGINE_API void SetName(std::string name) { m_name = name; }
	KENGINE_API void SetSize(uint32_t offset) { m_totalSize = offset; }
	KENGINE_API void SetStage(ShaderStage stage) { m_stage = stage; }

protected:
	std::string  m_name;
	ShaderStage m_stage;
	uint32_t m_totalSize;
};

class PipelineLayout : virtual public Core::IResource
{
public:
	virtual ~PipelineLayout() = default;

	KENGINE_API DescriptorSetLayoutHandle GetDescriptorSetLayout(std::string name) const;
	KENGINE_API DescriptorSetLayoutHandle GetDescriptorSetLayout(uint32_t index) const;
	KENGINE_API std::vector<DescriptorSetLayoutHandle>& GetAllDescriptorSetsLayouts() { return m_descriptorsLayouts; }
	KENGINE_API std::vector<std::string> GetAllDescriptorSetsLayoutsNames()const;
	KENGINE_API uint32_t GetDescriptorSetLayoutIndex(std::string name) const;

	KENGINE_API PushConstantLayoutHandle GetPushConstantLayouts(std::string name) const;
	KENGINE_API std::vector<PushConstantLayoutHandle>& GetAllPushConstantsLayouts() { return m_pushConstantsLayouts; }

	KENGINE_API void AddDescriptorSetLayout(DescriptorSetLayoutHandle layout) { m_descriptorsLayouts.push_back(layout); }
	KENGINE_API void AddPushConstantLayout(PushConstantLayoutHandle layout) { m_pushConstantsLayouts.push_back(layout); }

protected:
	std::vector<DescriptorSetLayoutHandle> m_descriptorsLayouts;
	std::vector<PushConstantLayoutHandle> m_pushConstantsLayouts;
};

class Pipeline : virtual public Core::IResource
{
public:
	virtual ~Pipeline() = default;

	KENGINE_API PipelineType GetType() { return m_type; }
	KENGINE_API PipelineLayoutHandle GetLayout() { return m_layout; }

	KENGINE_API void SetType(PipelineType type) { m_type = type; }
	KENGINE_API void SetLayout(PipelineLayoutHandle layout) { m_layout = layout; }


protected:
	PipelineType m_type;
	PipelineLayoutHandle m_layout;
};



END_NAMESPACE_CORE