#include "Core/RHI/Private/Vulkan/VulkanQueue.hpp"

QueueFamily QueueFamily::FindQueueFamily(const vk::PhysicalDevice& physicalDevice, std::vector<Queue> queues, bool searchPresentQueue, const vk::SurfaceKHR& surface)
{
    QueueFamily family;
    family.m_requestedQueues = queues;

    std::vector<vk::QueueFamilyProperties> properties = physicalDevice.getQueueFamilyProperties();

    std::unordered_set<Queue> requested(queues.begin(), queues.end());

    for (uint32_t i = 0; i < properties.size(); ++i)
    {
        // check for graphics
        if (requested.find(Queue::Graphics) != requested.end() && (properties[i].queueFlags & vk::QueueFlagBits::eGraphics))
        {
            family.m_queues[Queue::Graphics] = i;
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
        if (requested.find(Queue::Transfert) != requested.end() && (properties[i].queueFlags & vk::QueueFlagBits::eTransfer))
        {
            family.m_queues[Queue::Transfert] = i;
        }


        // check for compute
        if (requested.find(Queue::Compute) != requested.end() && (properties[i].queueFlags & vk::QueueFlagBits::eCompute))
        {
            family.m_queues[Queue::Compute] = i;
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
    for (Queue q : m_requestedQueues)
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