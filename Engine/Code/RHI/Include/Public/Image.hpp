#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct ImageSpecs
{

};

class Image : virtual public Core::IResource
{
public:
	virtual ~Image() = default;


};

END_NAMESPACE_RHI