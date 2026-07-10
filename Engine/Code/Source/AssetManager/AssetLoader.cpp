#include  "AssetLoader.hpp"

#include "spdlog/spdlog.h"

BEGIN_NAMESPACE_ASSETMANAGER

AssetLoader::AssetLoader(std::unordered_map<Priority, int> priorityQueuesThreadsCount)
{
    std::vector<Priority> priorities = std::vector<Priority>(priorityQueuesThreadsCount.size());
    uint32_t totalThreadsCount = 0;

    m_queueNames.reserve(priorityQueuesThreadsCount.size());
    for (const auto& [priority, threadCount] : priorityQueuesThreadsCount)
    {
        std::string queueName = "AssetLoaderQueue_" + std::to_string(static_cast<int>(priority));
        totalThreadsCount += threadCount;
        priorities.push_back(priority);

        m_queueNames.emplace(priority, queueName);
        m_pool.InitQueue(queueName, threadCount);
    }

    if (std::thread::hardware_concurrency() - m_minimumThreads < totalThreadsCount)
    {
        spdlog::warn("AssetLoader: Not enough threads available for the requested queues. Some queues may not have enough threads to function properly.");
    }
}

AssetLoader::~AssetLoader()
{
    //for (const auto& queueName : m_queueNames)
    //{
       // m_pool.ReleaseQueue(queueName);
    //}
}
END_NAMESPACE_ASSETMANAGER