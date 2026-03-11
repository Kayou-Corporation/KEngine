#include "Window/Window.hpp"

#include "Window/Private/SdlWindow.hpp"

#include <spdlog/spdlog.h>

BEGIN_NAMESPACE_CORE

RefCountPtr<Window> WindowInterface::InitWindow(WindowAPI api)
{
	switch (api)
	{
	case SDL:
		return CreateRefPtr<SDLWindow>();
		break;
	default:
		spdlog::critical("Can't create window");
		return nullptr;
		break;
	}
}

END_NAMESPACE_CORE