#pragma once

#include "RHI/API/RHI.hpp"
#include "Memory/KSharedPtr.hpp"

namespace Kayou::Window
{
	class Window;
}

BEGIN_NAMESPACE_CORE

class Instance;

struct SurfaceSpecs
{
	Core::KSharedPtr<Window::Window> window;
};

class Surface
{
public:
	virtual ~Surface() = default;
};

END_NAMESPACE_CORE