#pragma once

#include "Core.hpp"

#include <fstream>

BEGIN_NAMESPACE_CORE

template<typename T>
void Write(std::ofstream& out, const T& value)
{
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
void Read(std::ifstream& in, T& value)
{
    in.read(reinterpret_cast<char*>(&value), sizeof(T));
}

END_NAMESPACE_CORE