#pragma once
#include "CoreModule.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_CORE

#define ENGINE_QUEUE_ASSET "AssetLoaderQueue"
#define ENGINE_QUEUE_PHYSICS "PhysicsLoaderQueue"

class Engine : public virtual Core::IResource
{
public:
    explicit Engine(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount);
    virtual ~Engine() = default;

    // TO BE USED ONLY 1 TIME
    static void Init(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount);
    static void Shutdown();

    static Engine& Get();

    void TestFunction()
    {
        spdlog::error("Engine TestFunction called");
    }

public:
    Engine() = delete;
    Engine(Engine const&) = delete;
    Engine& operator=(Engine const&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

protected:

    // Device

    // ThreadPool
    ThreadPool m_pool;
    std::vector<std::string_view> m_queueNames;
    int m_minimumThreads = 2;

    // Assets



    inline static KUniquePtr<Engine> m_instance{nullptr};
};

END_NAMESPACE_CORE