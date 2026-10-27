#pragma once

#include "Asset.hpp"
#include "AssetManagerModule.hpp"
#include "Utils/Memory.hpp"
#include "Asset.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

#define ASSET_EXTENSION ".kasset"
#define DEFAULT_PATH "Cache/Assets/"

struct AssetHeader
{
    size_t size;

    uint32_t id;
    AssetType type;
    std::string name;
};

class AssetRegistry : public virtual Core::IResource
{
// Basic stuff for lifetime managment & access
public:
    KAPI explicit AssetRegistry();
    KAPI explicit AssetRegistry(const std::string_view& folderPath);
    KAPI virtual ~AssetRegistry() = default;

    // Deleted constructors / destructors
    AssetRegistry(const AssetRegistry&) = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;
    AssetRegistry(AssetRegistry&&) = delete;
    AssetRegistry& operator=(AssetRegistry&&) = delete;

public:
    KAPI void CreateAsset(const std::string& assetPath, const std::string& assetName, const AssetType type = AssetType::DefaultGuess);
    KAPI void DestroyAsset(uint32_t id);

    template<typename AssetDerived>
    KAPI SoftAssetRef<AssetDerived> GetAsset(uint32_t id)
    {
        if (id < 0)
        {
            spdlog::error("AssetRegistry::GetAsset: Invalid asset ID {}.", id);
            return SoftAssetRef<AssetDerived>{-1};
        }

        for (const auto& asset : m_assets)
        {
            if (asset->GetID() == id)
            {
                if (asset->GetType() != AssetDerived::GetStaticType())
                {
                    spdlog::error("AssetRegistry::GetAsset: Asset with ID {} is not of the requested type.", id);
                    return SoftAssetRef<AssetDerived>{id}; // Return an invalid SoftAssetRef
                }
                return SoftAssetRef<AssetDerived>{id};
            }
        }

        spdlog::error("AssetRegistry::GetAsset: Asset with ID {} not found.", id);
        return SoftAssetRef<AssetDerived>{-1}; // Return an invalid SoftAssetRef
    }

private:
    // High level functions, to be executed on initalization
    void RegisterAllAssets();
    void RegisterAsset(const std::string& assetPath);

    inline std::string CacheAssetPath(const std::string& name, const char* ext);

    // Maybe useless
    std::string m_folderPath;
    uint32_t m_lastAssetId = 0;

    std::vector<Core::KSharedPtr<Asset>> m_assets;
};
END_NAMESPACE_ASSETMANAGER


