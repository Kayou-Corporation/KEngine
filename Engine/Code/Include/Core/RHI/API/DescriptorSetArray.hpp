#pragma once

#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE

#define MAX_DESCRIPTORSETARRAY_SIZE 64

class DescriptorSetArray
{
public:
    DescriptorSetArray() = delete;
    DescriptorSetArray(std::string name) { m_name = name; }
    virtual ~DescriptorSetArray() = default;

    KAPI std::string GetName() const {return m_name; }

protected:
    std::string m_name;
};

END_NAMESPACE_CORE