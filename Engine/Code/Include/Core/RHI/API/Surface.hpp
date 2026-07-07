#pragma once

#include "RHI/API/RHI.hpp"

namespace Kayou::Window
{
	class Window;
}

BEGIN_NAMESPACE_RHI

class Instance;

struct SurfaceSpecs
{
	Core::RefCountPtr<Window::Window> window;
};

class Surface : virtual public Core::IResource
{
public:
	virtual ~Surface() = default;
};

END_NAMESPACE_RHI