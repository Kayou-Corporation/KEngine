#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE

struct SemaphoreSpecs
{
	SemaphoreType type = SemaphoreType::Binary;
	uint64_t timelineValue = 0;
};

class Semaphore
{
public:
	virtual ~Semaphore() = default;
};


class Fence
{
public:
	virtual ~Fence() = default;
};

END_NAMESPACE_CORE