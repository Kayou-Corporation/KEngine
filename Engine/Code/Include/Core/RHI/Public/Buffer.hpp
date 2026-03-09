#pragma once

#include "Core/Utils/Export.hpp"
#include "Core/Utils/Memory.hpp"

#include "Core/RHI/Public/RHI.hpp"

BEGIN_NAMESPACE_CORE

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

END_NAMESPACE_CORE