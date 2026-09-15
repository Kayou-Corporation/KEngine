#pragma once
#include "CoreModule.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_KAYOU

#define ENGINE_QUEUE_ASSET "AssetLoaderQueue"
#define ENGINE_QUEUE_PHYSICS "PhysicsLoaderQueue"

class Engine : public virtual Core::IResource
{
public:
    Engine() = delete;
    Engine(Engine const&) = delete;
    Engine& operator=(Engine const&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;
    ~Engine() {};

    // TO BE USED ONLY 1 TIME
    static void Init(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount);
    static void Shutdown();

    static Core::RefCountPtr<Engine> GetInstance();





protected:
    explicit Engine(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount);

    // Device

    // ThreadPool
    ThreadPool m_pool;
    std::vector<std::string_view> m_queueNames;
    int m_minimumThreads = 2;

    // Assets


    inline static Core::RefCountPtr<Engine> m_instance{nullptr};
};

END_NAMESPACE_KAYOU