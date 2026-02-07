#pragma once

#include <vector>

#include "Core/RHI/Public/RHI.hpp"

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
