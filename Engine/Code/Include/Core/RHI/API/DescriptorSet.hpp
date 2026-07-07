#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_RHI

class DescriptorSet : public virtual Core::IResource
{
public:
	virtual ~DescriptorSet() = default;
};

END_NAMESPACE_RHI