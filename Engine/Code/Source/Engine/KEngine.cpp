#include "../../Include/Engine/KEngine.hpp"

#include "RHI/API/Surface.hpp"
#include "spdlog/spdlog.h"
BEGIN_NAMESPACE_CORE

void KEngine::Init(const KEngineSpecs& specs)
{
    if (!m_instance)
    {
        m_instance = Core::MakeUnique<KEngine>(specs);
    }
    else
    {
        spdlog::error("Engine already initialized");
    }
}

void KEngine::Clean()
{
    if (!m_instance)
    {
        spdlog::error("Engine not initialized");
        return;
    }
    m_instance->CleanInternal();
}

void KEngine::Shutdown()
{
    if (!m_instance)
    {
        spdlog::error("Engine already shut down");
    }
    else
    {
        m_instance.Reset();
    }
}

void KEngine::Update()
{
    if (!m_instance)
    {
        return;
    }

    m_instance->m_assetRegistry->RunGarbageCollector();
}

KEngine& KEngine::Get()
{
    if (!m_instance)
    {
        spdlog::error("Engine not initialized");
    }
    return *m_instance;
}

void KEngine::CleanInternal()
{
    m_assetRegistry->UnloadAll();
}

KEngine::KEngine(const KEngineSpecs& specs)
{
    //----------- ThreadPool --------------//
    std::vector<Priority> priorities = std::vector<Priority>(specs.priorityQueuesThreadsCount.size());
    uint32_t totalThreadsCount = 0;

    m_queueNames.reserve(specs.priorityQueuesThreadsCount.size());
    for (const auto& [name, threadCount] : specs.priorityQueuesThreadsCount)
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

    //----------- AssetManager --------------//
    m_assetRegistry = Core::MakeUnique<AssetManager::AssetRegistry>("Cache/Assets/");
    m_assetRegistry->RegisterAllAssets();

    // ------------ Window -----------------//
    m_window = Window::WindowInterface::InitWindow(specs.windowAPI);

    Window::WindowSpecs windowSpecs;
    windowSpecs.width = specs.windowWidth;
    windowSpecs.height = specs.windowHeight;
    windowSpecs.name = specs.windowName;
    windowSpecs.allowResize = specs.allowWindowResize;
    windowSpecs.rendererAPI = specs.rendererAPI;

    m_window->Create(windowSpecs);

    // ------------ RHI Instance -----------------//
    m_rhiInstance = RendererInterface::InitRenderer(specs.rendererAPI);

    InstanceSpecs rhiInstanceSpecs;
    rhiInstanceSpecs.window = m_window;
    rhiInstanceSpecs.appVersion = specs.version;
    rhiInstanceSpecs.engineVersion = KENGINE_VERSION;
    rhiInstanceSpecs.debugLayers = specs.debugLayers;

    m_rhiInstance->Create(rhiInstanceSpecs);

    // ------------ RHI Surface -----------------//
    SurfaceSpecs windowSurfaceSpecs;
    windowSurfaceSpecs.window = m_window;

    m_windowSurface = m_rhiInstance->CreateSurface(windowSurfaceSpecs);

    // ------------ RHI Device -----------------//
    DeviceSpecs deviceSpecs;
    deviceSpecs.gpuType = GpuType::Discrete;
    deviceSpecs.extensions = {Core::Extensions::Swapchain, Core::Extensions::DynamicRendering, Core::Extensions::ShaderObject };
    deviceSpecs.queues = {Core::QueueType::Graphics };
    deviceSpecs.searchPresentQueue = true;
    deviceSpecs.surface = m_windowSurface;

    m_deviceHandle = m_rhiInstance->CreateDevice(deviceSpecs);
}

KEngine::~KEngine()
{
    m_assetRegistry->UnloadAll();

    m_rhiInstance->DestroySurface(m_windowSurface);
    m_rhiInstance->DestroyDevice(m_deviceHandle);
    m_rhiInstance->Destroy();

    m_window->Destroy();
}

END_NAMESPACE_CORE
