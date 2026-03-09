#include "Core/RHI/Public/Instance.hpp"

#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"

BEGIN_NAMESPACE_CORE

RefCountPtr<Instance> RendererInterface::InitRenderer(RendererAPI api)
{
	switch (api)
	{
	case Vulkan:
		return CreateRefPtr<VulkanInstance>();
	default:
		break;
	}

	return nullptr;
}

END_NAMESPACE_CORE