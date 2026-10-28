#pragma once

#include "CoreModule.hpp"

#include <fstream>
#include <type_traits>

BEGIN_NAMESPACE_CORE

template<typename T>
void Write(std::ofstream& out, const T& value)
{
    static_assert(std::is_trivially_copyable_v<T>, "Write requires T to be trivially copyable");

    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
void Read(std::ifstream& in, T& value)
{
    static_assert(std::is_trivially_copyable_v<T>, "Read requires T to be trivially copyable");

    in.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
inline bool IsFileOpenOrValid(T& file)
{
    static_assert((std::is_base_of<std::ifstream, T>::value || std::is_base_of<std::ofstream, T>::value || std::is_base_of<std::fstream, T>::value), "File object is not of right type. Right types are: std::ifstream, std::ofstream and std::fstream");

    return file.is_open() && file.good();
}

namespace InternalXXH32
{
    constexpr uint32_t PRIME32_1 = 2654435761U;
    constexpr uint32_t PRIME32_2 = 2246822519U;
    constexpr uint32_t PRIME32_3 = 3266489917U;
    constexpr uint32_t PRIME32_4 = 668265263U;
    constexpr uint32_t PRIME32_5 = 374761393U;

    inline uint32_t rotl32(uint32_t x, int r)
    {
        return (x << r) | (x >> (32 - r));
    }

    uint32_t ComputeHash(const char* input, size_t len, uint32_t seed = 0);
}

END_NAMESPACE_CORE