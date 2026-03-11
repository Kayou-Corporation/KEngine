#pragma once

#include "Core/Utils/Export.hpp"
#include "Core/Utils/Memory.hpp"

BEGIN_NAMESPACE_CORE

class CommandList : virtual public IResource
{
public:
	virtual ~CommandList() = default;

	// base functions to open close commandBuffer
	virtual void Open() = 0;
	virtual void Close() = 0;



	// Rendering
};

END_NAMESPACE_CORE