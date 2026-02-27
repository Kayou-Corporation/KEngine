#pragma once

#include "Core/Utils/Export.hpp"
#include "Core/Utils/Memory.hpp"

struct ImageSpecs
{

};

class Image : virtual public IResource
{
public:
	virtual ~Image() = default;


};