#pragma once

#include "Public/Device.hpp"

#include "Private/Vulkan/VulkanUtils.hpp"
#include "Private/Vulkan/VulkanQueue.hpp"

DISABLE_WARNINGS

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
    vk::PhysicalDeviceSynchronization2Features sync2Features;
    vk::PhysicalDeviceTimelineSemaphoreFeatures timelineSemaphore;
};

// Native extensions required for our API implementation
static std::vector<const char*> nativeExtensions =
{
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
};

class VulkanDevice : public Device
{
// Public
public:
    VulkanDevice();
    virtual ~VulkanDevice() override = default;

    //----------- Queue / Command --------------//
    virtual Core::RefCountPtr<CommandList> GetCommandList(QueueType queueType) override;
    virtual void SubmitCommandList(Core::RefCountPtr<CommandList> commandList, const SubmitInfo& submitInfo) override;
    virtual void WaitIdle() override;
    virtual void QueueWaitIdle(QueueType queueType) override;
    virtual void RunGarbageCollector() override;


    //----------- Syncronisation --------------// 
    virtual Core::RefCountPtr<Semaphore> CreateSemaphore(const SemaphoreSpecs& specs) override;
    virtual void DestroySemaphore(Core::RefCountPtr<Semaphore> semaphore) override;
    virtual void WaitForSemaphore(Core::RefCountPtr<Semaphore> semaphore, uint64_t waitValue) override;
    virtual Core::RefCountPtr<Fence> CreateFence() override;
    virtual void DestroyFence(Core::RefCountPtr<Fence> fence) override;
    virtual void WaitForFence(Core::RefCountPtr<Fence> fence) override;
    virtual void ResetFence(Core::RefCountPtr<Fence> fence) override;


    //----------- Swapchain --------------// 
    virtual Core::RefCountPtr<Swapchain> CreateSwapchain(const SwapchainSpecs& specs) override;
    virtual void DestroySwapchain(Core::RefCountPtr<Swapchain> swapchain) override;
    virtual uint32_t AcquirreNextImage(Core::RefCountPtr<Swapchain> swapchain, Core::RefCountPtr<Semaphore> Semaphore) override;
    virtual void Present(const PresentInfo& presentInfo) override;


    //-------------- Buffer --------------// 
    virtual Core::RefCountPtr<Buffer> CreateBuffer(const BufferSpecs& specs) override;
    virtual void DestroyBuffer(Core::RefCountPtr<Buffer> buffer) override;


    //-------------- Image --------------// 
    virtual Core::RefCountPtr<Image> CreateImage(const ImageSpecs& specs) override;
    virtual void DestroyImage(Core::RefCountPtr<Image> image) override;
    virtual std::vector<Core::RefCountPtr<Image>> CreatePresentationImages(Core::RefCountPtr<Swapchain> swapchain) override;
    virtual void DestroyPresentationImages(std::vector<Core::RefCountPtr<Image>> presentationImages) override;
    virtual Core::RefCountPtr<Image> CreateImagesWithSwapchain(const SwapchainImageSpecs& specs, Core::RefCountPtr<Swapchain> swapchain) override;

    // -------------- Shader -------------- // 
    virtual Core::RefCountPtr<Shader> CreateShader(const std::string& file, const ShaderStage& sStage, bool isGlobalLayout, bool usesGlobalLayout) override;
    virtual void DestroyShader(Core::RefCountPtr<Shader> shader) override;

    // -------------- DescriptorSetLayout -------------- // 
    virtual std::vector<Core::RefCountPtr<DescriptorSetLayout>> CreateDescriptorSetsLayouts(Core::RefCountPtr<Shader> shader) override;
    virtual Core::RefCountPtr<DescriptorSetLayout> CreateDescriptorSetLayout(Core::RefCountPtr<Shader> shader, std::string name) override;
    virtual Core::RefCountPtr<DescriptorSetLayout> CreateDescriptorSetLayout(Core::RefCountPtr<Shader> shader, uint32_t index) override;
    virtual void DestroyDescriptorSetsLayouts(std::vector<Core::RefCountPtr<DescriptorSetLayout>> descriptors) override;
    virtual void DestroyDescriptorSetLayout(Core::RefCountPtr<DescriptorSetLayout> descriptors) override;

    // -------------- PushConstantLayout -------------- // 
    virtual Core::RefCountPtr<PushConstantLayout> CreatePushConstantLayout(Core::RefCountPtr<Shader> shader) override;
    virtual void DestroyPushConstantsLayouts(std::vector<Core::RefCountPtr<PushConstantLayout>>& pushConstants) override;

    // -------------- Descriptor Set -------------- // 
    KENGINE_API virtual Core::RefCountPtr<DescriptorSet> CreateDescriptorSet(Core::RefCountPtr<DescriptorSetLayout> layout) override;
    KENGINE_API virtual void DestroyDescriptorSet(Core::RefCountPtr<DescriptorSet> descriptorSet) override;

    // -------------- Pipeline Layout -------------- // 
    virtual Core::RefCountPtr<PipelineLayout> CreatePipelineLayout(std::vector<Core::RefCountPtr<DescriptorSetLayout>> descriptors, std::vector<Core::RefCountPtr<PushConstantLayout>> pushConstants) override;
    virtual void DestroyPipelineLayout(Core::RefCountPtr<PipelineLayout>) override;

    // -------------- Pipeline -------------- // 
    virtual Core::RefCountPtr<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) override;
    // TODO : Implment compute pipeline
    //KENGINE_API virtual Core::RefCountPtr<ComputePipeline> CreateGraphicsPipeline(const ComputePipelineSpecs& specs) = 0;
    virtual void DestroyPipeline(Core::RefCountPtr<Pipeline> pipeline) override;


// Public vulkan
public:
    // Create vk::Device & vk::PhysicalDevice
    void PickPhysicalDevice(const vk::Instance& instance, const std::vector<QueueType>& queues, bool searchPresentQueue, const vk::SurfaceKHR& surface, vk::PhysicalDeviceType gpuType, std::vector<const char*> extensions);
    void CreateLogicalDevice(std::vector<const char*>& extensions);

    void CreateMemoryAllocator(const vk::Instance& instance);

    void Destroy();

    void DestroyBuffer(vk::Buffer buffer, VmaAllocation allocation);

    vk::Format CheckFormatCompatibility(vk::Format requestedFormat, vk::ImageTiling tiling, vk::FormatFeatureFlags requiredFeatures);

    vk::Device GetHandle() const { return m_handle; }
    VmaAllocator GetMemoryAllocator() const { return m_memoryAllocator; }

// Private Vulkan
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
};

END_NAMESPACE_RHI