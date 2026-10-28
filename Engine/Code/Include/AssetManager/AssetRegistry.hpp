#pragma once

#include "Asset.hpp"
#include "AssetManagerModule.hpp"
#include "Utils/Memory.hpp"

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

/*
 * Valid = Is valid, success
 *
 * InvalidName = create name -> success
 * NameAlreadyExist = Change name to add number : input : xxx -> xxx1 -> success
 * AssetAlreadyExist = Reimport asset
 *
 */
enum class AssetCreationRequest
{
    Valid = 0,

    InvalidName = 1,
    InvalidPath = 2,
    NameAlreadyExist = 3,
    //AssetAlreadyExist = 3,

    COUNT = 2
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
    // TODO : Implement this
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

    template<typename AssetDerived>
    KAPI SoftAssetRef<AssetDerived> GetAsset(const std::string& name)
    {
        if (name.empty())
        {
            spdlog::error("AssetRegistry::GetAsset: Invalid asset name.");
            return SoftAssetRef<AssetDerived>{-1};
        }

        for (const auto& asset : m_assets)
        {
            if (asset->GetName() == name)
            {
                if (asset->GetType() != AssetDerived::GetStaticType())
                {
                    spdlog::error("AssetRegistry::GetAsset: Asset with name {} is not of the requested type.", name);
                    return SoftAssetRef<AssetDerived>{asset->GetID()};
                }
                return SoftAssetRef<AssetDerived>{asset->GetID()};
            }
        }

        spdlog::error("AssetRegistry::GetAsset: Asset with name {} not found.", name);
        return SoftAssetRef<AssetDerived>{-1};
    }

    void RegisterAllAssets();

private:
    inline std::string CacheAssetPath(const std::string& name, const char* ext);

    // High level functions, to be executed on initalization
    // TODO : Implement this
    void RegisterAsset(const std::string& assetPath);

    AssetCreationRequest EnsureCreatedAssetValidity(const std::string& assetPath, const std::string& assetName);
    std::string GenerateFallbackName(const std::string& baseName);

    std::string m_folderPath;
    uint32_t m_lastAssetId = 0;

    std::vector<Core::KSharedPtr<Asset>> m_assets;
};
END_NAMESPACE_ASSETMANAGER


