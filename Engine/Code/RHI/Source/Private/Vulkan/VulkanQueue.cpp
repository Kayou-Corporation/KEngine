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

            std::cout << properties[i].queueCount << '\n';
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
    device.destroySemaphore(m_trackingSemaphore);
    
    VK_CHECK_VOID(m_handle.waitIdle(), "Queue can't wait");

    for (const auto& cmdBuffer : m_commandBuffersPool)
    {
        device.freeCommandBuffers(cmdBuffer->cmdPool, cmdBuffer->cmdBuffer);
        device.destroyCommandPool(cmdBuffer->cmdPool);
    }

    for (const auto& cmdBuffer : m_inFlightCommandBuffersPool)
    {
        device.freeCommandBuffers(cmdBuffer->cmdPool, cmdBuffer->cmdBuffer);
        device.destroyCommandPool(cmdBuffer->cmdPool);
    }

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
        m_commandBuffersPool.pop_front();
    }
    else
    {
        cmdBuffer = Core::CreateRefPtr<TrackedCommandBuffer>();
        
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

void Queue::Submit(TrackedCommandBufferPtr cmdBuffer)
{
    m_lastSubmitdId++;

    cmdBuffer->submissionId = m_lastSubmitdId;

    // Setup timeline semaphore for tracking
    vk::TimelineSemaphoreSubmitInfo timelineInfo;
    timelineInfo.setSignalSemaphoreValueCount(1);
    timelineInfo.setPSignalSemaphoreValues(&cmdBuffer->submissionId);

    vk::SubmitInfo submitInfo;
    submitInfo.setPNext(&timelineInfo);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&cmdBuffer->cmdBuffer);

    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(&m_trackingSemaphore);

    VK_CHECK_VOID(m_handle.submit(submitInfo, nullptr), "Can't submit command buffer");

    m_inFlightCommandBuffersPool.push_back(cmdBuffer);
}

void Queue::RunGarbageCollector(Core::RefCountPtr<VulkanDevice>& device)
{
    vk::Device vkDevice = device->GetHandle();

    std::list<TrackedCommandBufferPtr> submissions = std::move(m_inFlightCommandBuffersPool);

    m_lastFinishedId = VK_CHECK_RESULT(vkDevice.getSemaphoreCounterValue(m_trackingSemaphore), "Coudn't get semaphore value");

    for (const TrackedCommandBufferPtr& cmd : submissions)
    {

        if (cmd->submissionId <= m_lastFinishedId)
        {
            cmd->submissionId = 0;
            VK_CHECK_VOID(cmd->cmdBuffer.reset(), "Can't reset command buffer");

            TrackedStagingBufferPtr trackedStagingBuffer = cmd->trackedStagingBuffer;
            if (trackedStagingBuffer)
            {
                device->DestroyBuffer(trackedStagingBuffer->handle, trackedStagingBuffer->allocation);
                trackedStagingBuffer = {};
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