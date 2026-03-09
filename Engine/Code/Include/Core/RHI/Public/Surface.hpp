#pragma once

#include "Core/Utils/Memory.hpp"
#include "Core/Utils/Export.hpp"

BEGIN_NAMESPACE_CORE

class Window;
class Instance;

struct SurfaceSpecs
{
	RefCountPtr<Window> window;
};

class Surface : virtual public IResource
{
public:
	virtual ~Surface() = default;
};

END_NAMESPACE_CORE