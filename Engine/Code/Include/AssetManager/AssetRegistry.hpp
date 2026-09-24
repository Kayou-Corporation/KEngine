#pragma once

#include "AssetManagerModule.hpp"
#include  "StackAllocator.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

struct RegisterAsset
{
    uint32_t id;
    // Ref ?
};

class AssetRegistry : public virtual Core::IResource
{
// Basic stuff for lifetime managment & access
public:
    explicit AssetRegistry(const std::string_view& folderPath);
    virtual ~AssetRegistry() = default;

    // Deleted constructors / destructors
    AssetRegistry() = delete;
    AssetRegistry(const AssetRegistry&) = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;
    AssetRegistry(AssetRegistry&&) = delete;
    AssetRegistry& operator=(AssetRegistry&&) = delete;

    static void Init(const std::string_view &folderPath);
    static void Shutdown();
    static AssetRegistry& Get();

public:
    // To be executed on initalization
    void RegisterAllAssets();

    void CreateAsset(const std::string_view &assetPath);
    void DestroyAsset(uint32_t id);

private:
    inline static Core::KUniquePtr<AssetRegistry> m_instance{nullptr};

    std::string_view m_folderPath;
};
END_NAMESPACE_ASSETMANAGER


