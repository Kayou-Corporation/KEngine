#pragma once

#include "RHI/API/RHI.hpp"
#include <string>

BEGIN_NAMESPACE_CORE

class DescriptorSet
{
public:
	virtual ~DescriptorSet() = default;

protected:
	std::string name;
};

END_NAMESPACE_CORE