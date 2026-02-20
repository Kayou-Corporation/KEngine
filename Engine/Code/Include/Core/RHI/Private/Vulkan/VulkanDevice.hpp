#pragma once

#include "Core/RHI/Public/Device.hpp"

#include "Core/RHI/Private/Vulkan/VulkanUtils.hpp"
#include "Core/RHI/Private/Vulkan/VulkanQueue.hpp"

struct PhysicalDevice 
{
    vk::PhysicalDevice physicalDevice;
    QueueFamily family;
    uint32_t score;

    bool operator>(const PhysicalDevice& other) const
    {
        return score > other.score;
    }
};

struct PhysicalDeviceCompatibiliy
{
    vk::SurfaceCapabilitiesKHR capabilities{};
    std::vector<vk::SurfaceFormatKHR> formats{};
    std::vector<vk::PresentModeKHR> presentModes{};
};

struct DeviceFeatures
{
    vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
    vk::PhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeatures;
    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
    vk::PhysicalDeviceExtendedDynamicState2FeaturesEXT extendedDynamicState2Features;
};

class VulkanDevice : public Device
{
public:
    VulkanDevice() = default;
    virtual ~VulkanDevice() override = default;

    void PickPhysicalDevice(const vk::Instance& instance, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, std::vector<const char*> extensions);
    void CreateLogicalDevice(std::vector<const char*>& instanceDebugLayers);

    void Destroy();

private:
    PhysicalDevice RatePhysicalDevice(const vk::PhysicalDevice& physicalDevice, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, const std::vector<const char*>& requiredExtensions);
    void BuildFeaturesChain();

    vk::PhysicalDevice m_pDevice;
    PhysicalDeviceCompatibiliy m_compatibility{};

    QueueFamily m_queueFamily;
    std::unordered_map<QueueType, Queue> m_queues;

    std::vector<const char*> m_extensions;
    vk::PhysicalDeviceFeatures2 m_featuresChain;
    DeviceFeatures features;

    vk::Device m_handle;

    //vk::Queue presentQueue;
    //std::unordered_map<Queue, vk::Queue> m_availableQueues;
        
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

