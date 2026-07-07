#include "Window.hpp"

#include "SdlWindow.hpp"

#include <spdlog/spdlog.h>

BEGIN_NAMESPACE_WINDOW
	Core::RefCountPtr<Window> WindowInterface::InitWindow(WindowAPI api)
{
	switch (api)
	{
	case SDL:
		return Core::CreateRefPtr<SDLWindow>();
		break;
	default:
		spdlog::critical("Can't create window");
		return nullptr;
		break;
	}
}

END_NAMESPACE_WINDOW