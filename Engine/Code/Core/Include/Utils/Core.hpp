#pragma once

#define KAYOU_GLOBAL Kayou

#define BEGIN_NAMESPACE_KAYOU namespace KAYOU_GLOBAL {
#define END_NAMESPACE_KAYOU } /* namespace KAYOU_GLOBAL */

#define CORE_NAMESPACE Core

#define BEGIN_NAMESPACE_CORE BEGIN_NAMESPACE_KAYOU namespace CORE_NAMESPACE {
#define END_NAMESPACE_CORE } /* namespace CORE_NAMESPACE */ END_NAMESPACE_KAYOU

#if defined(_MSC_VER)
// It is not possible to disable all warnings for MSVC, so if the macro doesn't ignore a specific warning you want ignored, feel free to add the warning code below
#define DISABLE_ALL_WARNINGS __pragma(warning(push)) __pragma(warning(disable: 4100 4189 4244 4267 4456 4700 4701 4703 4996 4324))
#define RESTORE_WARNINGS     __pragma(warning(pop))
#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(x) _Pragma(#x)
#define DISABLE_ALL_WARNINGS \
        DO_PRAGMA(GCC diagnostic push) \
        DO_PRAGMA(GCC diagnostic ignored "-Wall") \
        DO_PRAGMA(GCC diagnostic ignored "-Wextra") \
        DO_PRAGMA(GCC diagnostic ignored "-Wpedantic")
#define RESTORE_WARNINGS DO_PRAGMA(GCC diagnostic pop)
#else
#define DISABLE_ALL_WARNINGS
#define RESTORE_WARNINGS
#endif

BEGIN_NAMESPACE_CORE

enum RendererAPI
{
    Vulkan = 0
};

END_NAMESPACE_CORE