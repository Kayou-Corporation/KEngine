#pragma once

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

struct SemaphoreSpecs
{
	SemaphoreType type = SemaphoreType::Binary;
	uint64_t timelineValue = 0;
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