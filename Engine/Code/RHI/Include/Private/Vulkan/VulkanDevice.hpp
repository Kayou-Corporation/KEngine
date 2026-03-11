#pragma once

#include "Public/Device.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanQueue.hpp"

DISABLE_ALL_WARNINGS

#include <vk_mem_alloc.h>

RESTORE_WARNINGS

BEGIN_NAMESPACE_RHI

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
    vk::PhysicalDeviceTimelineSemaphoreFeatures timelineSemaphore;
};

// Native extensions required for our API implementation
static std::vector<const char*> nativeExtensions =
{
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class VulkanDevice : public Device
{
// Public
public:
    VulkanDevice() = default;
    virtual ~VulkanDevice() override = default;

    // Commands / sync
    void WaitIdle() override;
    void QueueWaitIdle(QueueType type) override;

    // Create objects
    Core::RefCountPtr<Swapchain> CreateSwapchain(const SwapchainSpecs& specs) override;
    void DestroySwapchain(Core::RefCountPtr<Swapchain> swapchain) override;

    Core::RefCountPtr<Buffer> CreateBuffer(const BufferSpecs& specs) override;
    void DestroyBuffer(Core::RefCountPtr<Buffer> buffer) override;


// Public vulkan
public:
    // Create
    void PickPhysicalDevice(const vk::Instance& instance, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, std::vector<const char*> extensions);
    void CreateLogicalDevice(std::vector<const char*>& extensions);
    void CreateMemoryAllocator(const vk::Instance& instance);

    // Destroy
    void Destroy();



private:
    PhysicalDevice RatePhysicalDevice(const vk::PhysicalDevice& physicalDevice, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, const std::vector<const char*>& requiredExtensions);
    void BuildFeaturesChain();

    vk::PhysicalDevice m_pDevice;
    PhysicalDeviceCompatibiliy m_compatibility{};

    QueueFamily m_queueFamily;
    std::unordered_map<QueueType, Queue> m_queues;

    bool m_bSearchPresent = false;
    vk::Queue m_presentQueue;

    std::vector<const char*> m_extensions;
    vk::PhysicalDeviceFeatures2 m_featuresChain;
    DeviceFeatures features;

    vk::Device m_handle;

    VmaAllocator m_memoryAllocator;

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

END_NAMESPACE_RHI