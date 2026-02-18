#pragma once

#include "Core/Utils/Export.hpp"
#include "Core/Utils/Memory.hpp"

class CommandList : virtual public IResource
{
public:
	virtual ~CommandList() = default;
};