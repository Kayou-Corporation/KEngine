#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE

class DescriptorSet : public virtual Core::IResource
{
public:
	virtual ~DescriptorSet() = default;
};

END_NAMESPACE_CORE