#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE

class DescriptorSet
{
public:
	virtual ~DescriptorSet() = default;

protected:
	std::string name;
};

END_NAMESPACE_CORE