#pragma once

#include <vulkan/vulkan.hpp>

#include "Core/RHI/Public/Device.hpp"

struct PhysicalDeviceCompatibility
{
        
};

class VulkanDevice : public Device
{
public:
    VulkanDevice() = default;
    virtual ~VulkanDevice() override = default;

    void Create(const DeviceSpecs specs) override;
    void Destroy() override;

private:
    vk::PhysicalDevice m_pDevice;
    vk::Device m_handle;
        
};

/*
 DEVICE EXTENSIONS TO CHECK INTERNALLY : 

 - VK_KHR_dynamic_rendering // Dynamic Rendering

 - VK_EXT_shader_object // conseil d'utiliser en plus : 
                            VK_EXT_extended_dynamic_state
                            VK_EXT_extended_dynamic_state2
                            VK_EXT_extended_dynamic_state3

 - 
*/

struct NativeExtensions
{
    const char* DynamicRendering = "VK_KHR_dynamic_rendering";

    const char* ShaderObject = "VK_EXT_shader_object";
    const char* DynamicState = "VK_EXT_extended_dynamic_state";
    const char* DynamicState2 = "VK_EXT_extended_dynamic_state2";
    const char* DynamicState3 = "VK_EXT_extended_dynamic_state3";
};

