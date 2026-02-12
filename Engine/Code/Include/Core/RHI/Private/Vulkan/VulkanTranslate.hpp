#pragma once

#include <vector>

#include "Core/RHI/Public/RHI.hpp"


// ---------- DEBUG LAYERS ---------------
inline const char* TranslateToVulkan(DebugLayers layer)
{
    switch (layer)
    {
    case DebugLayers::Validation:
        return "VK_LAYER_KHRONOS_validation";

    case DebugLayers::DeviceMonitor:
        return "VK_LAYER_LUNARG_monitor";

    case DebugLayers::DeviceSimulation:
        return "VK_LAYER_LUNARG_device_simulation";

    case DebugLayers::APIDump:
        return "VK_LAYER_LUNARG_api_dump";
    }

    return nullptr;
}

inline std::vector<const char*> TranslateToVulkan(const std::vector<DebugLayers>& layers)
{
    std::vector<const char*> vkDebugLayers;

    for (uint32_t i = 0; i < layers.size(); ++i)
    {
        switch (layers[i])
        {
        case DebugLayers::Validation:
            vkDebugLayers.push_back("VK_LAYER_KHRONOS_validation");
            break;

        case DebugLayers::DeviceMonitor:
            vkDebugLayers.push_back("VK_LAYER_LUNARG_monitor");
            break;

        case DebugLayers::DeviceSimulation:
            vkDebugLayers.push_back("VK_LAYER_LUNARG_device_simulation");
            break;

        case DebugLayers::APIDump:
            vkDebugLayers.push_back("VK_LAYER_LUNARG_api_dump");
            break;
        }
    }

    return vkDebugLayers;
}

// ---------- EXTENSIONS ---------------
inline const char* TranslateToVulkan(Extensions extension)
{
    switch (extension)
    {
    case Extensions::Swapchain:
        return "VK_KHR_swapchain";

    case Extensions::DynamicRendering:
        return "VK_KHR_dynamic_rendering";

    case Extensions::ShaderObject:
        return "VK_EXT_shader_object";

    case Extensions::ExtendedDynamicState:
        return "VK_EXT_extended_dynamic_state";

    case Extensions::ExtendedDynamicState2:
        return "VK_EXT_extended_dynamic_state2";
    }

    return nullptr;
}

inline std::vector<const char*> TranslateToVulkan(const std::vector<Extensions>& extension)
{
    std::vector<const char*> vkExtensions;

    for (uint32_t i = 0; i < extension.size(); ++i)
    {
        switch (extension[i])
        {
        case Extensions::Swapchain:
            vkExtensions.push_back("VK_KHR_swapchain");
            break;

        case Extensions::DynamicRendering:
            vkExtensions.push_back("VK_KHR_dynamic_rendering");
            break;

        case Extensions::ShaderObject:
            vkExtensions.push_back("VK_EXT_shader_object");
            break;

        case Extensions::ExtendedDynamicState:
            vkExtensions.push_back("VK_EXT_extended_dynamic_state");
            break;

        case Extensions::ExtendedDynamicState2:
            vkExtensions.push_back("VK_EXT_extended_dynamic_state2");
            break;
        }
    }

    return vkExtensions;
}

// ---------- Features ---------------

// ---------- GPU Type ----------------
inline vk::PhysicalDeviceType TranslateToVulkan(GpuType gpuType)
{
    switch (gpuType)
    {
    case GpuType::Discrete :
        return vk::PhysicalDeviceType::eDiscreteGpu;

    case GpuType::Virtual:
        return vk::PhysicalDeviceType::eVirtualGpu;

    case GpuType::Cpu:
        return vk::PhysicalDeviceType::eCpu;
    }

    return vk::PhysicalDeviceType::eOther;
}