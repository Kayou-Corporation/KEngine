#pragma once

#include "RHI/API/RHI.hpp"
#include <string>

BEGIN_NAMESPACE_CORE

class StandaloneDescriptorSet
{
public:
	virtual ~StandaloneDescriptorSet() = default;

	std::string GetName() const { return name; }
protected:
	std::string name;
};

END_NAMESPACE_CORE