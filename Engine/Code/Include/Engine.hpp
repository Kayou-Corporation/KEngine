#pragma once
#include "CoreModule.hpp"

//#include "ThreadPool.hpp"

BEGIN_NAMESPACE_KAYOU

#define ENGINE_QUEUE_ASSET "AssetLoaderQueue"
#define ENGINE_QUEUE_PHYSICS "PhysicsLoaderQueue"

class Engine
{
public:
    //Engine(std::unordered_map<std::string_view, int> priorityQueuesThreadsCount);
    ~Engine();



private:
    // Device

    // ThreadPool
    //ThreadPool m_pool;
    //std::vector<std::string_view> m_queueNames;
    int m_minimumThreads = 2;

    // Assets


private:
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;
};

END_NAMESPACE_KAYOU