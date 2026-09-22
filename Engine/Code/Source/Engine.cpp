#include "Engine.hpp"

#include "spdlog/spdlog.h"
BEGIN_NAMESPACE_KAYOU

void Engine::Init(const std::unordered_map<std::string_view, int> &priorityQueuesThreadsCount)
{
    if (!m_instance)
    {
        m_instance = Core::CreateRefPtr<Engine>(priorityQueuesThreadsCount);
    }
    else
    {
        spdlog::error("Engine already initialized");
    }
}

void Engine::Shutdown()
{
    if (!m_instance)
    {
        spdlog::error("Engine already shut down");
    }
}

Core::KSharedPtr<Engine> Engine::GetInstance()
{
    if (!m_instance)
    {
        spdlog::error("Engine not initialized");
    }
    return m_instance;
}

Engine::Engine(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount)
{
    //----------- ThreadPool --------------//
    std::vector<Priority> priorities = std::vector<Priority>(priorityQueuesThreadsCount.size());
    uint32_t totalThreadsCount = 0;

    m_queueNames.reserve(priorityQueuesThreadsCount.size());
    for (const auto& [name, threadCount] : priorityQueuesThreadsCount)
    {
        std::string queueName = std::string(name);
        totalThreadsCount += threadCount;
        m_queueNames.push_back(queueName);

        m_pool.InitQueue(queueName, static_cast<uint8_t>(threadCount));
    }

    if (std::thread::hardware_concurrency() - m_minimumThreads < totalThreadsCount)
    {
        spdlog::warn("AssetLoader: Not enough threads available for the requested queues. Some queues may not have enough threads to function properly.");
    }

    
}

END_NAMESPACE_KAYOU