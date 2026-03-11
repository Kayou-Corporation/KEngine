#pragma once

#include "Core/Utils/Export.hpp"
#include "Core/Utils/Memory.hpp"

BEGIN_NAMESPACE_CORE

struct ImageSpecs
{

};

class Image : virtual public IResource
{
public:
	virtual ~Image() = default;


};

END_NAMESPACE_CORE