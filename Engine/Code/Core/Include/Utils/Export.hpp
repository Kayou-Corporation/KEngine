#pragma once

#include <iostream>
#include <cstring>

#ifndef ENGINEEXPORT_H
#define ENGINEEXPORT_H

#ifdef Engine_EXPORTS
#if defined _MSC_VER && not defined(__clang__)
#define KENGINE_API __declspec(dllexport)
#else
#define KENGINE_API __attribute__((visibility("default")))
#endif
#else
#if defined _MSC_VER && not defined(__clang__)
#define KENGINE_API __declspec(dllimport)
#else
#define KENGINE_API
#endif
#endif

#endif

#if (defined(_DEBUG) || defined(DEBUG) || defined(__DEBUG__) || defined(__DEBUG)) && not defined(NDEGUG)
#define KENGINE_DEBUG
#endif

#if defined _MSC_VER && not defined(__clang__)
#define BREAKPOINT() __debugbreak()
#else
#define BREAKPOINT() __builtin_trap()
#endif

#ifdef KENGINE_DEBUG
#if defined _MSC_VER && not defined(__clang__)
#define BREAKPOINT() __debugbreak()
#else
#define BREAKPOINT() __builtin_trap()
#endif
#define ASSERT(condition, message) \
					do { \
						if (!(condition)) { \
							std::cerr << "Assertion failed: " << (message) << " in file " << (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) << " at line " << __LINE__ << '\n'; \
							BREAKPOINT(); \
						} \
					} while(0)
#else
#define ASSERT(condition, message) \
					do { \
						if (!(condition)) { \
							std::cerr << "Assertion failed: " << (message) << " in file " << (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) << " at line " << __LINE__ << '\n'; \
							abort(); \
						} \
					} while(0)
#endif
