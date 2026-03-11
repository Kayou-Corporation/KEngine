#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct BufferSpecs
{
	std::vector<BufferUsage> usages;
	std::vector<MemoryAccess> memoryAccess;
	uint32_t size;
};

class Buffer : public virtual Core::IResource
{
public:
	virtual ~Buffer() = default;

	
};

END_NAMESPACE_RHI