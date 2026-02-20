#include "Core/RHI/Private/Vulkan/VulkanQueue.hpp"
#include "Core/RHI/Private/Vulkan/VulkanCommandList.hpp"

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

void Queue::Create(vk::Device& device)
{
    vk::SemaphoreTypeCreateInfo typeInfo(vk::SemaphoreType::eTimeline, 0);
    vk::SemaphoreCreateInfo certInfo;
    certInfo.pNext = &typeInfo;

    trackingSemaphore = VK_CHECK_RESULT(device.createSemaphore(certInfo), "Coudn't create tracking semaphore");
}

void Queue::Destroy(vk::Device& device)
{
    device.destroySemaphore(trackingSemaphore);
    
    VK_CHECK_VOID(handle.waitIdle(), "Queue can't wait");

    //for (const auto& cmdBuffer : commandBuffersPool)
    //{
    //    //cmdBuffer
    //}
}

RefCountPtr<VulkanCommandList> Queue::GetOrCreateCommandBuffer(vk::Device& device)
{
    RefCountPtr<VulkanCommandList> cmdList = CreateRefPtr<VulkanCommandList>();

    if (!commandBuffersPool.empty())
    {
        cmdList->SetHandle(commandBuffersPool.front());
        commandBuffersPool.pop_front();
    }
    else
    {
        TrackedCommandBufferPtr cmdBuffer = CreateRefPtr<TrackedCommandBuffer>();
        
        vk::CommandPoolCreateInfo poolInfo;
        cmdBuffer->cmdPool = VK_CHECK_RESULT(device.createCommandPool(poolInfo), "Coudn't create command pool");
        
        vk::CommandBufferAllocateInfo bufferInfo;
        bufferInfo.level = vk::CommandBufferLevel::ePrimary;
        bufferInfo.commandPool = cmdBuffer->cmdPool;
        bufferInfo.commandBufferCount = 1;
        cmdBuffer->cmdBuffer = VK_CHECK_RESULT(device.allocateCommandBuffers(bufferInfo), "Coudn't allocate command buffer")[0];
    
        cmdList->SetHandle(cmdBuffer);
    }

	return cmdList;
}

void Queue::Submit(RefCountPtr<VulkanCommandList> cmdList)
{
    lastSubmitdId++;

    TrackedCommandBufferPtr cmd = cmdList->GetHandle();
    cmd->submissionId = lastSubmitdId;

    // Setup timeline semaphore for tracking
    vk::TimelineSemaphoreSubmitInfo timelineInfo;
    timelineInfo.setSignalSemaphoreValueCount(1);
    timelineInfo.setPSignalSemaphoreValues(&cmd->submissionId);

    vk::SubmitInfo submitInfo;
    submitInfo.setPNext(&timelineInfo);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&cmd->cmdBuffer);

    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(&trackingSemaphore);

    VK_CHECK_VOID(handle.submit(submitInfo, nullptr), "Can't submit command buffer");

    inFlightCommandBuffersPool.push_back(cmd);
}

void Queue::RunGarbageCollector(vk::Device& device)
{
    std::list<TrackedCommandBufferPtr> submissions = std::move(inFlightCommandBuffersPool);

    lastFinishedId = VK_CHECK_RESULT(device.getSemaphoreCounterValue(trackingSemaphore), "Coudn't get semaphore value");

    for (const TrackedCommandBufferPtr& cmd : submissions)
    {

        if (cmd->submissionId <= lastFinishedId)
        {
            cmd->submissionId = 0;
            commandBuffersPool.push_back(cmd);
        }
        else
        {
            inFlightCommandBuffersPool.push_back(cmd);
        }
    }
}