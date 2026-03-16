#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

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
};

class Buffer : public virtual Core::IResource
{
public:
	virtual ~Buffer() = default;

	// Warning : will return null if the buffer is GPU only
	KENGINE_API virtual void* GetMappedData() const = 0;

	KENGINE_API virtual bool GetIsGpuOnly() const { return m_isGpuOnly; }

	KENGINE_API virtual uint32_t GetSize() const { return m_size; }

protected:
	uint32_t m_size;
	bool m_isGpuOnly = false;
};

END_NAMESPACE_RHI