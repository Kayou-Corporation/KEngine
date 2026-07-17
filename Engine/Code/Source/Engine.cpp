#include "Engine.hpp"

#include "spdlog/spdlog.h"
BEGIN_NAMESPACE_KAYOU

Engine::Engine(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount)
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

        m_pool.InitQueue(queueName, static_cast<uint8_t>(threadCount));
    }

    if (std::thread::hardware_concurrency() - m_minimumThreads < totalThreadsCount)
    {
        spdlog::warn("AssetLoader: Not enough threads available for the requested queues. Some queues may not have enough threads to function properly.");
    }

    
}

// TODO : Replace with destructor when needed
Engine::~Engine() = default;

END_NAMESPACE_KAYOU