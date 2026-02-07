#pragma once

#include <iostream>
#include <cstring>

#include "Core/Utils/Export.hpp"

#ifdef KENGINE_DEBUG

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

// define debug mode vkCheck
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

