#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

class Buffer;
class Image;

class DescriptorSet : public virtual Core::IResource
{
public:
	virtual ~DescriptorSet() = default;
};

END_NAMESPACE_RHI