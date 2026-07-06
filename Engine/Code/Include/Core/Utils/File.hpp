#pragma once

#include "Core.hpp"

#include <fstream>
#include <type_traits>

BEGIN_NAMESPACE_CORE

template<typename T>
void Write(std::ofstream& out, const T& value)
{
    static_assert(std::is_trivially_copyable_v<T>, "Core::Write requires T to be trivially copyable");

    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
void Read(std::ifstream& in, T& value)
{
    static_assert(std::is_trivially_copyable_v<T>, "Core::Read requires T to be trivially copyable");

    in.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
inline bool IsFileOpenOrValid(T& file)
{
    static_assert((std::is_base_of<std::ifstream, T>::value || std::is_base_of<std::ofstream, T>::value || std::is_base_of<std::fstream, T>::value), "File object is not of right type. Right types are: std::ifstream, std::ofstream and std::fstream");

    return file.is_open() && file.good();
}

END_NAMESPACE_CORE