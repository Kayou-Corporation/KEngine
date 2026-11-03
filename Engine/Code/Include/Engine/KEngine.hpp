#pragma once
#include "AssetRegistry.hpp"
#include "CoreModule.hpp"
#include "SoftAssetReference.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_CORE

#define ENGINE_QUEUE_ASSET "AssetLoaderQueue"
#define ENGINE_QUEUE_PHYSICS "PhysicsLoaderQueue"

class KEngine : public virtual Core::IResource
{
public:
    explicit KEngine(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount);
    virtual ~KEngine() = default;

    // TO BE USED ONLY 1 TIME
    static void Init(const std::unordered_map<std::string_view, int>& priorityQueuesThreadsCount);
    static void Shutdown();
    static void Update();

    static KEngine& Get();

    void CreateAsset(const std::string& assetPath, const std::string& assetName, const AssetManager::AssetType type = AssetManager::AssetType::DefaultGuess)
    {
        m_assetRegistry->CreateAsset(assetPath, assetName, type);
    }

    void DestroyAsset(const std::string& name)
    {
        m_assetRegistry->DestroyAsset(name);
    }

    template<typename AssetDerived>
    AssetManager::KSoftAssetRef<AssetDerived> GetAsset(uint32_t id)
    {
        return m_assetRegistry->GetAsset<AssetDerived>(id);
    }

    template<typename AssetDerived>
    AssetManager::KSoftAssetRef<AssetDerived> GetAsset(const std::string& name)
    {
        return m_assetRegistry->GetAsset<AssetDerived>(name);
    }

public:
    KEngine() = delete;
    KEngine(KEngine const&) = delete;
    KEngine& operator=(KEngine const&) = delete;
    KEngine(KEngine&&) = delete;
    KEngine& operator=(KEngine&&) = delete;

protected:
    // Device

    // ThreadPool
    ThreadPool m_pool;
    std::vector<std::string_view> m_queueNames;
    int m_minimumThreads = 2;

    // Assets
    Core::KUniquePtr<AssetManager::AssetRegistry> m_assetRegistry;


    // Self reference for singleton
    inline static KUniquePtr<KEngine> m_instance{nullptr};
};

END_NAMESPACE_CORE