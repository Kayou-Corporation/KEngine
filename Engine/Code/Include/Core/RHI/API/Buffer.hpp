#pragma once

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

class Device;

struct BufferSpecs
{
	uint32_t size;
	BufferUsage primaryUsage;
	std::vector<BufferUsage> additionalUsages;
	MemoryAccess memoryAccess;
	PipelineStage pipelineStage;
	// isPersistentMapped is not relevant for GpuOnly buffer !
	bool isPersistentMapped = true;
};

class Buffer : public virtual Core::IResource
{
public:
	virtual ~Buffer() = default;

	// Warning : will return null if the buffer is GPU only
	KENGINE_API virtual void* GetMappedData() const = 0;

	virtual bool GetIsGpuOnly() const { return m_isGpuOnly; }
	virtual MemoryAccess GetMemoryAccess() const { return m_access; }

	virtual uint32_t GetSize() const { return m_size; }

	virtual bool GetIsPersistentMapped() const { return m_isPersistentMapped; }

protected:
	uint32_t m_size;
	MemoryAccess m_access;
	bool m_isGpuOnly = false;
	bool m_isPersistentMapped = false;
};

END_NAMESPACE_RHI