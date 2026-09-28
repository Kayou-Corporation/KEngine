#pragma once

#include "Asset.hpp"
#include "AssetManagerModule.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

#define ASSET_EXTENSION ".kasset"

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

public:
    void Init();
    void ShutDown();

    void CreateAsset(const std::string& assetPath);
    void DestroyAsset(uint32_t id);

private:
    // High level functions, to be executed on initalization
    void RegisterAllAssets();
    void RegisterAsset(const std::string_view& assetPath);

    template<typename RawData>
    void WriteAssetRawData(std::ofstream& file, const RawData& data);

    inline std::string CacheAssetPath(const std::string& name, const char* ext);

    std::string_view m_folderPath;
    uint32_t m_lastAssetId = 0;
};
END_NAMESPACE_ASSETMANAGER


