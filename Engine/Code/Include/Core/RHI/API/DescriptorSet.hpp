#pragma once
#include <string>

#include "CoreModule.hpp"
#include "Export.hpp"

BEGIN_NAMESPACE_CORE

class DescriptorSet
{
public:
    DescriptorSet() = delete;
    DescriptorSet(std::string name) { m_name = name; }
    virtual ~DescriptorSet() = default;

    KAPI std::string GetName() const {return m_name; }

protected:
    std::string m_name;
};

END_NAMESPACE_CORE