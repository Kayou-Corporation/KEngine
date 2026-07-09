#pragma once

#include <iostream>
#include <cstring>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#include "Export.hpp"
#include "BuildMode.hpp"
#include "Debug.hpp"

#ifdef KDEBUG

#define VK_CHECK_RESULT(func, message)                                       \
    ([&]() {                                                                 \
        auto _res = (func);                                                  \
        if (_res.result != vk::Result::eSuccess) {                           \
            std::cerr << "Vulkan Error: " << vk::to_string(_res.result)      \
                      << " | " << (message)                                   \
                      << " | File: "                                         \
                      << (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) \
                      << " | Line: " << __LINE__ << '\n';                    \
            BREAKPOINT();                                                    \
        }                                                                    \
        return _res.value;                                                   \
    })()

#else

#define VK_CHECK_RESULT(func, message)                                         \
    ([&]() {                                                                   \
        auto _res = (func);                                                    \
        if (_res.result != vk::Result::eSuccess) {                             \
            std::cerr << "Vulkan Error: " << vk::to_string(_res.result)        \
                      << " | " << (message)                                     \
                      << " | File: "                                           \
                      << (strrchr(__FILE__, '\\')                              \
                             ? strrchr(__FILE__, '\\') + 1                     \
                             : __FILE__)                                       \
                      << " | Line: " << __LINE__ << '\n';                      \
            abort();                                                           \
        }                                                                      \
        return _res.value;                                                     \
    })()

#endif

#ifdef KDEBUG
#define VK_CHECK_VOID(func, message)                                             \
    do {                                                                             \
        vk::Result _code = (func);                                                   \
        if (_code != vk::Result::eSuccess) {                                         \
            std::cerr << "[Vulkan Error]\n"                                          \
                      << "  Result  : " << vk::to_string(_code) << "\n"              \
                      << "  Message : " << (message) << "\n"                         \
                      << "  File    : "                                              \
                      << (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 :    \
                         (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)) \
                      << "\n  Line    : " << __LINE__ << std::endl;                  \
            BREAKPOINT();                                                            \
        }                                                                            \
    } while (0)
#else
#define VK_CHECK_VOID(func, message)                                             \
    do {                                                                             \
        if ((func) != vk::Result::eSuccess) {                                        \
            abort();                                                                 \
        }                                                                            \
    } while (0)
#endif

