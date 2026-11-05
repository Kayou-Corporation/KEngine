#pragma once
#include "CoreModule.hpp"

#include "Utils/Memory.hpp"
#include "AssetRegistry.hpp"
#include "SoftAssetReference.hpp"

#include "Window.hpp"
#include "WindowRenderer.hpp"
#include "RHI/API/Surface.hpp"

#include "ThreadPool.hpp"

#include "RHI/API/Instance.hpp"
#include "RHI/API/Device.hpp"
#include "RHI/API/RHI.hpp"

BEGIN_NAMESPACE_CORE
#define ENGINE_QUEUE_ASSET "AssetLoaderQueue"
#define ENGINE_QUEUE_PHYSICS "PhysicsLoaderQueue"

#define KENGINE_VERSION Version(0, 0, 1)

struct KEngineSpecs
{
    Window::WindowAPI windowAPI;
    uint32_t windowWidth;
    uint32_t windowHeight;
    std::string windowName;
    bool allowWindowResize;

    Version version;

    std::unordered_map<std::string_view, int> priorityQueuesThreadsCount;

    RendererAPI rendererAPI;
    std::vector<DebugLayers> debugLayers;
};

class KEngine : public virtual Core::IResource
{
public:
    explicit KEngine(const KEngineSpecs& specs);
    virtual ~KEngine();

    // TO BE USED ONLY 1 TIME
    static void Init(const KEngineSpecs& specs);
    static void Clean();
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

    KSharedPtr<Window::Window> GetWindow() const
    {
        return m_window;
    }

    SurfaceHandle GetWindowSurface() const
    {
        return m_windowSurface;
    }

    DeviceHandle GetDevice() const
    {
        return m_deviceHandle;
    }

public:
    KEngine() = delete;
    KEngine(KEngine const&) = delete;
    KEngine& operator=(KEngine const&) = delete;
    KEngine(KEngine&&) = delete;
    KEngine& operator=(KEngine&&) = delete;

protected:
    // Window :
    KSharedPtr<Window::Window> m_window;
    SurfaceHandle m_windowSurface;

    // RHI :
    InstanceHandle m_rhiInstance;
    DeviceHandle m_deviceHandle;

    // ThreadPool
    ThreadPool m_pool;
    std::vector<std::string_view> m_queueNames;
    int m_minimumThreads = 2;

    // Assets
    KUniquePtr<AssetManager::AssetRegistry> m_assetRegistry;


    // Self reference for singleton
    inline static KUniquePtr<KEngine> m_instance{nullptr};

private:
    void CleanInternal();
};

END_NAMESPACE_CORE