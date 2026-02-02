#pragma once

#ifndef ENGINEEXPORT_H
#define ENGINEEXPORT_H

#ifdef Engine_EXPORTS
#ifdef _MSC_VER
#define KENGINE_API __declspec(dllexport)
#else
#define KENGINE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define KENGINE_API __declspec(dllimport)
#else
#define KENGINE_API
#endif
#endif

#endif

#if defined(_DEBUG) || defined(DEBUG) || defined(__DEBUG__) || defined(__DEBUG)
#define KENGINE_DEBUG
#else
#define KENGINE_DEBUG
#endif