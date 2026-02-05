#include "Core/RHI/Private/Vulkan/VulkanInstance.hpp"

#include "Window/Window.hpp"

void VulkanInstance::Create(const InstanceSpecs& specs)
{
	(void)specs;
	RefCountPtr<WindowRenderer> wRenderer = specs.window->GetWindowRenderer();
	
	RefCountPtr<VulkanWindowRenderer> wvkRenderer = wRenderer.UnsafeCastAs<VulkanWindowRenderer>();
	std::vector<const char*> vkExtensions = wvkRenderer->GetVulkanInstanceExtensions();


	//wRenderer
	//auto specs.window->GetWindowRenderer();
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
