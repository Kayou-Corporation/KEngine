#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

class CommandList : virtual public Core::IResource
{
public:
	virtual ~CommandList() = default;

	// base functions to open close commandBuffer
	virtual void Open() = 0;
	virtual void Close() = 0;



	// Rendering
};

END_NAMESPACE_RHI