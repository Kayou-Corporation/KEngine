#include "Window.hpp"

#include "SdlWindow.hpp"

#include <spdlog/spdlog.h>

#include "Memory/KSharedPtr.hpp"

BEGIN_NAMESPACE_WINDOW
	Core::KSharedPtr<Window> WindowInterface::InitWindow(WindowAPI api)
{
	switch (api)
	{
	case SDL:
		return Core::MakeShared<SDLWindow>();
		break;
	default:
		spdlog::critical("Can't create window");
		return nullptr;
		break;
	}
}

END_NAMESPACE_WINDOW