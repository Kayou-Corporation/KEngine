#include "Engine.hpp"

#include "spdlog/spdlog.h"

BEGIN_NAMESPACE_KAYOU

/*Engine::Engine(std::unordered_map<std::string_view, int> priorityQueuesThreadsCount)
{
    //----------- ThreadPool --------------//
    std::vector<Priority> priorities = std::vector<Priority>(priorityQueuesThreadsCount.size());
    uint32_t totalThreadsCount = 0;

    m_queueNames.reserve(priorityQueuesThreadsCount.size());
    for (const auto& [name, threadCount] : priorityQueuesThreadsCount)
    {
        std::string queueName = "AssetLoaderQueue_" + std::string(name);
        totalThreadsCount += threadCount;
        m_queueNames.push_back(queueName);

        m_pool.InitQueue(queueName, threadCount);
    }

    if (std::thread::hardware_concurrency() - m_minimumThreads < totalThreadsCount)
    {
        spdlog::warn("AssetLoader: Not enough threads available for the requested queues. Some queues may not have enough threads to function properly.");
    }

    
}

Engine::~Engine()
{

}*/

END_NAMESPACE_KAYOU