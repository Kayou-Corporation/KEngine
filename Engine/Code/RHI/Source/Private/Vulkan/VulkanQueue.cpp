#include "Private/Vulkan/VulkanQueue.hpp"
#include "Private/Vulkan/VulkanCommandList.hpp"
#include "Private/Vulkan/VulkanDevice.hpp"

BEGIN_NAMESPACE_RHI

QueueFamily QueueFamily::FindQueueFamily(const vk::PhysicalDevice& physicalDevice, std::vector<QueueType> queues, bool searchPresentQueue, const vk::SurfaceKHR& surface)
{
    QueueFamily family;
    family.m_requestedQueues = queues;

    std::vector<vk::QueueFamilyProperties> properties = physicalDevice.getQueueFamilyProperties();

    std::unordered_set<QueueType> requested(queues.begin(), queues.end());

    for (uint32_t i = 0; i < properties.size(); ++i)
    {
        // check for graphics
        if (requested.find(QueueType::Graphics) != requested.end() && (properties[i].queueFlags & vk::QueueFlagBits::eGraphics))
        {
            family.m_queues[QueueType::Graphics] = i;
        }

        //// check for present
        if (searchPresentQueue)
        {
            if (VK_CHECK_RESULT(physicalDevice.getSurfaceSupportKHR(i, surface), "Coudn't check if surface support presentation"))
            {
                family.m_presentQueue = i;
                family.m_searchPresentQueue = true;
            }
        }

        // check for transfert
        if (requested.find(QueueType::Transfer) != requested.end() && (properties[i].queueFlags & vk::QueueFlagBits::eTransfer))
        {
            family.m_queues[QueueType::Transfer] = i;
        }


        // check for compute
        if (requested.find(QueueType::Compute) != requested.end() && (properties[i].queueFlags & vk::QueueFlagBits::eCompute))
        {
            family.m_queues[QueueType::Compute] = i;
        }

        // check if family has all requested queues
        if (family.IsComplete())
        {
            break;
        }
    }

    return family;
}

bool QueueFamily::IsComplete() const
{
    for (QueueType q : m_requestedQueues)
    {
        auto it = m_queues.find(q);
        if (it == m_queues.end() || !it->second.has_value())
        {
            return false;
        }
    }

    if (m_searchPresentQueue)
    {
        if (!m_presentQueue.has_value())
            return false;
    }
    return true;
}

void Queue::Create(vk::Device& device, vk::Queue& queue, uint32_t index, vk::QueueFlagBits type)
{
    m_handle = queue;
    m_queueFamilyIndex = index;
    m_queueType = type;

    vk::SemaphoreTypeCreateInfo typeInfo(vk::SemaphoreType::eTimeline, 0);
    vk::SemaphoreCreateInfo certInfo;
    certInfo.pNext = &typeInfo;

    m_trackingSemaphore = VK_CHECK_RESULT(device.createSemaphore(certInfo), "Coudn't create tracking semaphore");
}

void Queue::Destroy(vk::Device& device)
{
    
    VK_CHECK_VOID(m_handle.waitIdle(), "Queue can't wait");

    for (const auto& cmdBuffer : m_commandBuffersPool)
    {
        device.freeCommandBuffers(cmdBuffer->cmdPool, cmdBuffer->cmdBuffer);
        device.destroyCommandPool(cmdBuffer->cmdPool);

        TrackedStagingBufferPtr trackedStagingBuffer = cmdBuffer->trackedStagingBuffer;
        if (trackedStagingBuffer)
        {
            vmaDestroyBuffer(m_memoryAllocator, trackedStagingBuffer->handle, trackedStagingBuffer->allocation);
            cmdBuffer->trackedStagingBuffer = {};
        }
    }

    for (const auto& cmdBuffer : m_inFlightCommandBuffersPool)
    {
        device.freeCommandBuffers(cmdBuffer->cmdPool, cmdBuffer->cmdBuffer);
        device.destroyCommandPool(cmdBuffer->cmdPool);

        TrackedStagingBufferPtr trackedStagingBuffer = cmdBuffer->trackedStagingBuffer;
        if (trackedStagingBuffer)
        {
            vmaDestroyBuffer(m_memoryAllocator, trackedStagingBuffer->handle, trackedStagingBuffer->allocation);
            cmdBuffer->trackedStagingBuffer = {};
        }
    }

    device.destroySemaphore(m_trackingSemaphore);

    m_commandBuffersPool.clear();
    m_inFlightCommandBuffersPool.clear();
}

void Queue::WaitIdle()
{
    VK_CHECK_VOID(m_handle.waitIdle(), "Queue can't wait");
}

TrackedCommandBufferPtr Queue::GetOrCreateCommandBuffer(vk::Device& device)
{
    TrackedCommandBufferPtr cmdBuffer;

    if (!m_commandBuffersPool.empty())
    {
        cmdBuffer = m_commandBuffersPool.front();
        cmdBuffer->memoryAllocator = m_memoryAllocator;
        m_commandBuffersPool.pop_front();
    }
    else
    {
        cmdBuffer = Core::CreateRefPtr<TrackedCommandBuffer>();
        cmdBuffer->memoryAllocator = m_memoryAllocator;

        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.queueFamilyIndex = m_queueFamilyIndex;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        cmdBuffer->cmdPool = VK_CHECK_RESULT(device.createCommandPool(poolInfo), "Coudn't create command pool");
        
        vk::CommandBufferAllocateInfo bufferInfo;
        bufferInfo.level = vk::CommandBufferLevel::ePrimary;
        bufferInfo.commandPool = cmdBuffer->cmdPool;
        bufferInfo.commandBufferCount = 1;
        cmdBuffer->cmdBuffer = VK_CHECK_RESULT(device.allocateCommandBuffers(bufferInfo), "Coudn't allocate command buffer")[0];
    }

	return cmdBuffer;
}

void Queue::Submit(TrackedCommandBufferPtr cmdBuffer, vk::PipelineStageFlags waitStages)
{
    m_lastSubmitdId++;

    cmdBuffer->submissionId = m_lastSubmitdId;

    // Setup timeline semaphore for tracking
    m_signalSemaprhores.push_back(m_trackingSemaphore);
    m_signalSemaphoresValues.push_back(cmdBuffer->submissionId);

    std::vector<vk::PipelineStageFlags> waitMasks(m_waitSemaprhores.size(), waitStages);

    vk::TimelineSemaphoreSubmitInfo timelineInfo;
    timelineInfo.setSignalSemaphoreValueCount(m_signalSemaprhores.size());
    timelineInfo.setPSignalSemaphoreValues(m_signalSemaphoresValues.data());
    timelineInfo.setWaitSemaphoreValueCount(m_waitSemaprhores.size());
    timelineInfo.setPWaitSemaphoreValues(m_waitSemaprhoresValues.data());

    vk::SubmitInfo submitInfo;
    submitInfo.setPNext(&timelineInfo);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&cmdBuffer->cmdBuffer);

    submitInfo.setSignalSemaphoreCount(m_signalSemaprhores.size());
    submitInfo.setPSignalSemaphores(m_signalSemaprhores.data());

    submitInfo.setWaitSemaphoreCount(m_waitSemaprhores.size());
    submitInfo.setPWaitSemaphores(m_waitSemaprhores.data());

    submitInfo.setPWaitDstStageMask(waitMasks.data());

    VK_CHECK_VOID(m_handle.submit(submitInfo), "Can't submit command buffer");

    m_signalSemaprhores.clear();
    m_signalSemaphoresValues.clear();

    m_waitSemaprhores.clear();
    m_waitSemaprhoresValues.clear();

    m_inFlightCommandBuffersPool.push_back(cmdBuffer);
}

void Queue::PushSignalSemaphore(const vk::Semaphore& semaphore, uint64_t value)
{ 
    m_signalSemaprhores.push_back(semaphore); 
    m_signalSemaphoresValues.push_back(value);
}

void Queue::PushSignalSemaphores(const std::vector<vk::Semaphore>& semaphores, const std::vector<uint64_t>& values)
{
    m_signalSemaprhores.reserve(m_signalSemaprhores.size() + semaphores.size());
    for (const auto& s : semaphores) 
    {
        m_signalSemaprhores.push_back(s);
    }

    m_signalSemaphoresValues.insert(m_signalSemaphoresValues.end(), values.begin(), values.end());
   // m_signalSemaprhores.insert(m_signalSemaprhores.end(), semaphores.begin(), semaphores.end());
   // m_signalSemaphoresValues.insert(m_signalSemaphoresValues.end(), values.begin(), values.end());
}

void Queue::PushWaitSemaphore(const vk::Semaphore& semaphore, uint64_t value)
{ 
    m_waitSemaprhores.push_back(semaphore); 
    m_waitSemaprhoresValues.push_back(value);
}
void Queue::PushWaitSemaphores(const std::vector<vk::Semaphore>& semaphores, const std::vector<uint64_t>& values)
{ 
    m_waitSemaprhores.reserve(m_waitSemaprhores.size() + semaphores.size());
    for (const auto& s : semaphores)
    {
        m_waitSemaprhores.push_back(s);
    }

    m_waitSemaprhoresValues.insert(m_waitSemaprhoresValues.end(), values.begin(), values.end());
}

void Queue::RunGarbageCollector(vk::Device& device)
{
    std::list<TrackedCommandBufferPtr> submissions = std::move(m_inFlightCommandBuffersPool);

    m_lastFinishedId = VK_CHECK_RESULT(device.getSemaphoreCounterValue(m_trackingSemaphore), "Coudn't get semaphore value");

    for (const TrackedCommandBufferPtr& cmd : submissions)
    {

        if (cmd->submissionId <= m_lastFinishedId)
        {
            cmd->submissionId = 0;
            VK_CHECK_VOID(cmd->cmdBuffer.reset(), "Can't reset command buffer");

            TrackedStagingBufferPtr trackedStagingBuffer = cmd->trackedStagingBuffer;
            if (trackedStagingBuffer)
            {
                vmaDestroyBuffer(m_memoryAllocator, trackedStagingBuffer->handle, trackedStagingBuffer->allocation);
                cmd->trackedStagingBuffer = {};
            }

            m_commandBuffersPool.push_back(cmd);
        }
        else
        {
            m_inFlightCommandBuffersPool.push_back(cmd);
        }
    }
}

END_NAMESPACE_RHI