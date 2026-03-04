#pragma once

#include "Core/Utils/Export.hpp"
#include "Core/Utils/Memory.hpp"

#include "Core/RHI/Public/RHI.hpp"

struct BufferSpecs
{
	BufferUsage usage;
	std::vector<MemoryAccess> memoryAccess;
};

class Buffer : public virtual IResource
{
public:
	virtual ~Buffer() = default;

	
};