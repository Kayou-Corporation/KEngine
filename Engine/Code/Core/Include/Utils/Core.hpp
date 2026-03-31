#pragma once

#define KAYOU_GLOBAL Kayou

#define BEGIN_NAMESPACE_KAYOU namespace KAYOU_GLOBAL {
#define END_NAMESPACE_KAYOU } /* namespace KAYOU_GLOBAL */

#define CORE_NAMESPACE Core

#define BEGIN_NAMESPACE_CORE BEGIN_NAMESPACE_KAYOU namespace CORE_NAMESPACE {
#define END_NAMESPACE_CORE } /* namespace CORE_NAMESPACE */ END_NAMESPACE_KAYOU


#if defined(__clang__) || defined(__GNUC__)
#define DO_PRAGMA(x) _Pragma(#x)
// Some warning flags might need to be added, see the flag in the output to add it here
#define DISABLE_WARNINGS \
        DO_PRAGMA(GCC diagnostic push) \
        DO_PRAGMA(GCC diagnostic ignored "-Wall") \
        DO_PRAGMA(GCC diagnostic ignored "-Wextra") \
        DO_PRAGMA(GCC diagnostic ignored "-Wpedantic") \
        DO_PRAGMA(GCC diagnostic ignored "-Wunused-parameter")
        DO_PRAGMA(GCC diagnostic ignored "-Wunused-variable")
        DO_PRAGMA(GCC diagnostic ignored "-Wmissing-field-initializers")
        DO_PRAGMA(GCC diagnostic ignored "-Wnullability-completeness")
#define RESTORE_WARNINGS DO_PRAGMA(GCC diagnostic pop)
#elif defined(_MSC_VER)
// Some warning flags might need to be added, see the flag in the output to add it here
#define DISABLE_WARNINGS __pragma(warning(push)) __pragma(warning(disable: 4100 4189 4244 4267 4456 4700 4701 4703 4996 4324))
#define RESTORE_WARNINGS __pragma(warning(pop))
#else
#define DISABLE_WARNINGS
#define RESTORE_WARNINGS
#endif

BEGIN_NAMESPACE_CORE

enum class RendererAPI : uint8_t
{
    Vulkan = 0
};

END_NAMESPACE_CORE