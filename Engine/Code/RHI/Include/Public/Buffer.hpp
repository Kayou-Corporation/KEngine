#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct BufferSpecs
{
	std::vector<BufferUsage> usages;
	MemoryAccess memoryAccess;
	uint32_t size;
};

class Buffer : public virtual Core::IResource
{
public:
	virtual ~Buffer() = default;

	virtual void SetData(void* data, uint32_t size) = 0;
	// Warning : will return null if the buffer is GPU only
	virtual void* GetMappedData() const = 0;

	uint32_t GetSize() const { return m_size; }

protected:
	uint32_t m_size;
};

END_NAMESPACE_RHI