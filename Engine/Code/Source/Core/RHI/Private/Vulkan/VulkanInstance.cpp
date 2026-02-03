#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"

#include "Window/Window.hpp"

void VulkanInstance::Create(const InstanceSpecs& specs)
{
	(void)specs;
	std::vector<const char*> vkExtensions;
}

void VulkanInstance::Destroy()
{

}

//RefCountPtr<Surface> VulkanInstance::CreateSurface(RefCountPtr<Window> window)
//{
//	return nullptr;
//}
//
//void VulkanInstance::DestroySurface(RefCountPtr<Surface>)
//{
//
//}
