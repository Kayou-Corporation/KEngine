#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct SemaphoreSpecs
{
	SemaphoreType type;
	uint64_t timelineValue;
};

class Semaphore : virtual public Core::IResource
{
public:
	virtual ~Semaphore() = default;
};



class Fence : virtual public Core::IResource
{
public:
	virtual ~Fence() = default;
};

END_NAMESPACE_RHI