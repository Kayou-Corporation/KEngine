#include "Public/Instance.hpp"

#include "Private/Vulkan/VulkanInstance.hpp"

BEGIN_NAMESPACE_RHI

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

END_NAMESPACE_RHI