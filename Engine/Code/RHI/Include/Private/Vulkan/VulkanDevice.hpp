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
public:
    vk::PhysicalDevice physicalDevice;
    QueueFamily family;
    uint32_t score = 0;

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
    virtual CommandListHandle GetCommandList(QueueType queueType) override;
    virtual void SubmitCommandList(CommandListHandle commandList, const SubmitInfo& submitInfo) override;
    virtual void WaitIdle() override;
    virtual void QueueWaitIdle(QueueType queueType) override;
    virtual void RunGarbageCollector() override;


    //----------- Syncronisation --------------// 
    virtual SemaphoreHandle CreateSemaphore(const SemaphoreSpecs& specs) override;
    virtual void DestroySemaphore(SemaphoreHandle semaphore) override;
    virtual void WaitForSemaphore(SemaphoreHandle semaphore, uint64_t waitValue) override;
    virtual FenceHandle CreateFence() override;
    virtual void DestroyFence(FenceHandle fence) override;
    virtual void WaitForFence(FenceHandle fence) override;
    virtual void ResetFence(FenceHandle fence) override;


    //----------- Swapchain --------------// 
    virtual Core::RefCountPtr<Swapchain> CreateSwapchain(const SwapchainSpecs& specs) override;
    virtual void DestroySwapchain(Core::RefCountPtr<Swapchain> swapchain) override;
    virtual uint32_t AcquirreNextImage(Core::RefCountPtr<Swapchain> swapchain, SemaphoreHandle Semaphore) override;
    virtual bool Present(const PresentInfo& presentInfo) override;


    //-------------- Buffer --------------// 
    virtual BufferHandle CreateBuffer(const BufferSpecs& specs) override;
    virtual void DestroyBuffer(BufferHandle buffer) override;


    //-------------- Image --------------// 
    virtual Core::RefCountPtr<Image> CreateImage(const ImageSpecs& specs) override;
    virtual void DestroyImage(Core::RefCountPtr<Image> image) override;
    virtual std::vector<Core::RefCountPtr<Image>> CreatePresentationImages(Core::RefCountPtr<Swapchain> swapchain) override;
    virtual void DestroyPresentationImages(std::vector<Core::RefCountPtr<Image>> presentationImages) override;
    virtual Core::RefCountPtr<Image> CreateImagesWithSwapchain(const SwapchainImageSpecs& specs, Core::RefCountPtr<Swapchain> swapchain) override;

    // -------------- Sampler -------------- // 
    virtual SamplerHandle CreateSampler(const SamplerSpecs& specs) override;
    virtual void DestroySampler(SamplerHandle sampler) override;

    // -------------- Shader -------------- // 
    virtual ShaderHandle CreateShader(const std::string& file, const ShaderStage& sStage, bool isGlobalLayout, bool usesGlobalLayout) override;
    virtual void DestroyShader(ShaderHandle shader) override;

    // -------------- DescriptorSetLayout -------------- // 
    virtual std::vector<DescriptorSetLayoutHandle> CreateDescriptorSetsLayouts(ShaderHandle shader) override;
    virtual DescriptorSetLayoutHandle CreateDescriptorSetLayout(ShaderHandle shader, std::string name) override;
    virtual DescriptorSetLayoutHandle CreateDescriptorSetLayout(ShaderHandle shader, uint32_t index) override;
    virtual void DestroyDescriptorSetsLayouts(std::vector<DescriptorSetLayoutHandle> descriptors) override;
    virtual void DestroyDescriptorSetLayout(DescriptorSetLayoutHandle descriptors) override;

    // -------------- PushConstantLayout -------------- // 
    virtual PushConstantLayoutHandle CreatePushConstantLayout(ShaderHandle shader) override;
    virtual void DestroyPushConstantsLayouts(std::vector<PushConstantLayoutHandle>& pushConstants) override;

    // -------------- Descriptor Set -------------- // 
    virtual DescriptorSetHandle CreateDescriptorSet(DescriptorSetLayoutHandle layout) override;
    virtual void DestroyDescriptorSet(DescriptorSetHandle descriptorSet) override;
    virtual void SetDescriptorSetBuffer(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, BufferHandle buffer, uint32_t offset, uint32_t range) override;
    virtual void SetDescriptorSetBuffer(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, BufferHandle buffer, uint32_t offset, uint32_t range) override;
    virtual void SetDescriptorSetImage(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, Core::RefCountPtr<Image> image, SamplerHandle sampler) override;
    virtual void SetDescriptorSetImage(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, Core::RefCountPtr<Image> image, SamplerHandle sampler) override;
    virtual void SetDescriptorSetSampler(DescriptorSetHandle descriptorSet, std::string name, DescriptorType type, SamplerHandle sampler) override;
    virtual void SetDescriptorSetSampler(DescriptorSetHandle descriptorSet, uint32_t index, DescriptorType type, SamplerHandle sampler) override;

    // -------------- Pipeline Layout -------------- // 
    virtual PipelineLayoutHandle CreatePipelineLayout(std::vector<DescriptorSetLayoutHandle> descriptors, std::vector<PushConstantLayoutHandle> pushConstants) override;
    virtual void DestroyPipelineLayout(PipelineLayoutHandle) override;

    // -------------- Pipeline -------------- // 
    virtual GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineSpecs& specs) override;
    // TODO : Implment compute pipeline
    //KENGINE_API virtual Core::RefCountPtr<ComputePipeline> CreateGraphicsPipeline(const ComputePipelineSpecs& specs) = 0;
    virtual void DestroyPipeline(PipelineHandle pipeline) override;

    virtual void UpdateCompatibility(Core::RefCountPtr<Surface> surface) override;

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