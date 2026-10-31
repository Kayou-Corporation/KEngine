#pragma once

#include "AssetManager.hpp"
#include "AssetManagerModule.hpp"
#include "Export.hpp"
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
    KAPI void DestroyAsset(uint32_t id);

    KAPI bool LoadAsset(uint32_t id);
    KAPI void UnloadAsset(uint32_t id);

    template<typename AssetDerived>
    KAPI Core::KSharedPtr<AssetDerived> GetAsset(uint32_t id)
    {
        auto it = m_assetsById.find(id);
        if (it == m_assetsById.end())
        {
            spdlog::error("AssetRegistry::GetAsset: Asset ID {} non trouve.", id);
            return nullptr;
        }

        uint32_t index = it->second;
        Core::KSharedPtr<Asset>& asset = m_assets[index];

        if (!asset) return nullptr;

        if (!asset->IsLoaded())
        {
            if (!LoadAsset(id))
            {
                spdlog::error("AssetRegistry::GetAsset: Fail to load Asset {}.", id);
                return nullptr;
            }
        }

        return asset.CastAs<AssetDerived>();
    }

    void RegisterAllAssets();

private:
    Core::KSharedPtr<Asset> CreateAssetInstance(AssetType type, const std::string& name, uint32_t id);
    inline std::string CacheAssetPath(const std::string& name, const char* ext);

    // High level functions, to be executed on initalization
    void RegisterAsset(const std::string& assetPath);

    AssetCreationRequest EnsureCreatedAssetValidity(const std::string& assetPath, const std::string& assetName);
    std::string GenerateFallbackName(const std::string& baseName);

    std::string m_folderPath;
    uint32_t m_lastAssetId = 0;

    std::vector<Core::KSharedPtr<Asset>> m_assets;
    std::unordered_map<uint32_t, uint32_t> m_assetsById;
    std::vector<uint32_t> m_freeIndex;
};
END_NAMESPACE_ASSETMANAGER


