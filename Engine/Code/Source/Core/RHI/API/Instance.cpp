#include "RHI/API/Instance.hpp"

#include "RHI/Vulkan/VulkanInstance.hpp"

BEGIN_NAMESPACE_CORE

InstanceHandle RendererInterface::InitRenderer(Core::RendererAPI api)
{
	switch (api)
	{
	case Core::RendererAPI::Vulkan:
		return Core::CreateRefPtr<VulkanInstance>();
	default:
		break;
	}

	return nullptr;
}

END_NAMESPACE_CORE