#pragma once

#include <map>
#include <unordered_map>

#include "CoreModule.hpp"

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE

struct DescriptorPoolSpecs
{
    std::unordered_map<DescriptorType, uint32_t> descriptors;
    uint32_t maxDescriptorSetsCount;
    bool poolForArray = false;
};

class DescriptorPool
{
public:
    DescriptorPool() = default;
    virtual ~DescriptorPool() = default;

};

END_NAMESPACE_CORE
