#include "Core/RHI/Public/Instance.hpp"

#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"

RefCountPtr<Instance> RendererInterface::InitWindow(RendererAPI api)
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