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

RefCountPtr<VulkanCommandList> Queue::GetOrCreateCommandBuffer(vk::Device& device)
{
    RefCountPtr<VulkanCommandList> cmdList = CreateRefPtr<VulkanCommandList>();

    vk::CommandBuffer cmbBuffer;
    vk::Fence fence;

    // Get a vk::CommandBuffer
    if (idleCommandBuffers.empty())
    {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool = cmdPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        auto cmdBufferVector = VK_CHECK_RESULT(device.allocateCommandBuffers(allocInfo), "Can't allocate command buffer");

        cmbBuffer = cmdBufferVector[0];
    }
    else
    {
        cmbBuffer = idleCommandBuffers.back();
        idleCommandBuffers.pop_back();
    }

    // Get a vk::Fence 
    if (idleFences.empty())
    {
        vk::FenceCreateInfo fenceInfo;
        fence = VK_CHECK_RESULT(device.createFence(fenceInfo), "Can't create fence");
    }
    else
    {
        fence = idleFences.back();
        idleFences.pop_back();
        // Not sure, i want to reset in garbage not in acquire 
        //VK_CHECK_VOID(device.resetFences(1, &fence), "Can't reset fence");
    }

    // Push items in inFlight to mark them as currently use
    inFlightCommandBuffers.push_back(cmbBuffer);
    inFlightFences.push_back(fence);

    // Get an Index for tracking 
    uint32_t activeIndex = static_cast<uint32_t>(inFlightCommandBuffers.size() - 1);

    cmdList->SetHandle(cmbBuffer);
    cmdList->SetIndex(activeIndex);

	return cmdList;
}

void Queue::Submit(RefCountPtr<VulkanCommandList> cmdList)
{
    vk::CommandBuffer& cmdBuffer = cmdList->GetHandleRef();
    uint32_t bufferIndex = cmdList->GetIndex();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VK_CHECK_VOID(handle.submit(submitInfo, inFlightFences[bufferIndex]), "Can't submit command buffer");
}

void Queue::RunGarbageCollector(vk::Device& device)
{
    for (uint32_t i = 0; i < inFlightFences.size(); ++i)
    {
        vk::Fence& fence = inFlightFences[i];
        vk::Result status = device.getFenceStatus(fence);

        // Fence is ready, we can recycle our fence and associated command buffer
        if (status == vk::Result::eSuccess)
        {
            VK_CHECK_VOID(device.resetFences(1, &fence), "Can't reset fence");
            VK_CHECK_VOID(inFlightCommandBuffers[i].reset(), "Can't reset command buffer");
        }
        // Error other than the fence not being finish
        else if (status != vk::Result::eNotReady)
        {
            VK_CHECK_VOID(status, "Fence error while verifying status");
        }
    }
}