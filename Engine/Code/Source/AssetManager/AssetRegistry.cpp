#include  "AssetRegistry.hpp"

#include <fstream>
#include <filesystem>

#include "ExternalAssetLoader.hpp"
#include "InternalAssetSerializer.hpp"
#include "StaticMesh.hpp"
#include  "Utils/File.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

AssetRegistry::AssetRegistry()
{
    m_folderPath = DEFAULT_PATH;
    m_assets.reserve(50);
}

AssetRegistry::AssetRegistry(const std::string_view &folderPath)
{
    m_folderPath = folderPath;
    m_assets.reserve(50);
}


    // TODO : Implement this
    void AssetRegistry::RegisterAllAssets()
    {
        std::filesystem::path folderPath(m_folderPath);

        if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
        {
            spdlog::error("AssetRegistry::RegisterAllAssets: Invalid folder path: {}", m_folderPath);
            return;
        }

        for (const auto &entry: std::filesystem::directory_iterator(folderPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == ASSET_EXTENSION)
            {
                RegisterAsset(entry.path().string());
            }
        }
    }

    Core::KSharedPtr<Asset> AssetRegistry::CreateAssetInstance(AssetType type, const std::string &name, uint32_t id)
    {
        switch (type)
        {
            case AssetType::StaticMesh:
                return Core::CreateRefPtr<CoreObject::StaticMesh>(type, name, id, 0, 0);
            default:
                spdlog::error("AssetRegistry: Type d'asset non supporte.");
                return nullptr;
        }
    }

    void AssetRegistry::CreateAsset(const std::string &assetPath, const std::string &assetName, const AssetType type)
    {
        // Create only 1 time, become void if called multiple times
        std::filesystem::create_directories(m_folderPath);

        AssetCreationRequest assetCreationRequest = EnsureCreatedAssetValidity(assetPath, assetName);

        std::string usedName = assetName;
        if (assetCreationRequest == AssetCreationRequest::NameAlreadyExist)
        {
            usedName = GenerateFallbackName(assetName);
        }

        //else if (assetCreationRequest == AssetCreationRequest::InvalidPath)
        //{
        //    return;
        //}
        //else if (assetCreationRequest == AssetCreationRequest::InvalidName)
        //{
        //    spdlog::error("AssetRegistry::CreateAsset: Invalid asset name: {}", assetName);
        //    return;
        //}

        // Source asset info
        //uint32_t sourceSize = ExternalAssetLoader::GetAssetSize(assetPath);
        //uint32_t sourceHash = Core::InternalXXH32::ComputeHash(assetPath.c_str(), assetPath.length());

        // Actual asset created
        const std::string fullAssetPath = CacheAssetPath(usedName, ASSET_EXTENSION);
        std::ofstream asset(fullAssetPath, std::ios::binary);

        // Type of asset
        AssetType usedAssetType;
        if (type == AssetType::DefaultGuess)
        {
            usedAssetType = ExternalAssetLoader::GuessAssetTypeFromSource(assetPath);
        } else
        {
            usedAssetType = type;
        }

        // Write asset header
        size_t commonDataSize = sizeof(uint32_t) + sizeof(AssetType) + sizeof(uint32_t) + (
                                    sizeof(char) * usedName.length());
        AssetHeader header =
        {
            .size = commonDataSize,
            .id = m_lastAssetId,
            .type = usedAssetType,
            .name = usedName
        };
        InternalAssetSerializer::WriteAssetHeader(asset, header);

        switch (usedAssetType)
        {
            case AssetType::StaticMesh:
            {
                RawStaticMeshData assetData = ExternalAssetLoader::LoadAssetFromSource<RawStaticMeshData>(assetPath);
                InternalAssetSerializer::WriteAssetRawData(asset, assetData);
                spdlog::info("AssetRegistry::CreateAsset: Creating StaticMesh asset from path: {}", assetPath);
                break;
            }
            case AssetType::Texture2D:
            {
                // TODO: Implement for texture
                RawTextureData textureData = ExternalAssetLoader::LoadAssetFromSource<RawTextureData>(assetPath);
                //InternalAssetSerializer::WriteAssetRawData(asset, textureData);
                spdlog::info("AssetRegistry::CreateAsset: Creating Texture asset from path: {}", assetPath);
                break;
            }
            default:
                spdlog::error("AssetRegistry::CreateAsset: Unsupported asset type for path: {}", assetPath);
                return;
        }

        // Create Asset in memory :
        Core::KSharedPtr<AssetManager::Asset> assetPtr = CreateAssetInstance(usedAssetType, usedName, m_lastAssetId);

        m_assets.push_back(std::move(assetPtr));
        m_assetsById[m_lastAssetId] = m_assets.size() - 1;

        m_lastAssetId++;
    }

    void AssetRegistry::RegisterAsset(const std::string &assetPath)
    {
        std::ifstream asset(assetPath, std::ios::binary);
        AssetHeader header = InternalAssetSerializer::ReadAssetHeader(asset);

        Core::KSharedPtr<AssetManager::Asset> assetPtr = CreateAssetInstance(header.type, header.name, header.id);

        m_assets.push_back(std::move(assetPtr));
        m_assetsById[header.id] = m_assets.size() - 1;
    }

    AssetCreationRequest AssetRegistry::EnsureCreatedAssetValidity(const std::string &assetPath,
                                                                   const std::string &assetName)
    {
        // Invalid input
        if (assetPath.empty())
        {
            spdlog::error("AssetRegistry::EnsureCreatedAssetValidity: Invalid asset path.");
            return AssetCreationRequest::InvalidPath;
        }

        if (assetName.empty())
        {
            spdlog::error("AssetRegistry::EnsureCreatedAssetValidity: Invalid asset name.");
            return AssetCreationRequest::InvalidName;
        }

        // already has name
        for (const auto &asset: m_assets)
        {
            if (asset->GetName() == assetName)
            {
                spdlog::warn("AssetRegistry::EnsureCreatedAssetValidity: Asset with name '{}' already exists.",
                             assetName);
                return AssetCreationRequest::NameAlreadyExist;
            }
        }


        return AssetCreationRequest::Valid;
    }

std::string AssetRegistry::GenerateFallbackName(const std::string &baseName)
{
    return baseName + "1";
}

std::string AssetRegistry::CacheAssetPath(const std::string &name, const char *ext)
{
    return m_folderPath + name + ext;
}

void AssetRegistry::DestroyAsset(uint32_t id)
{
    auto it = m_assetsById.find(id);
    if (it == m_assetsById.end())
    {
        spdlog::warn("AssetRegistry::DestroyAsset: Asset ID {} not found.", id);
        return;
    }

    if (m_assetsById[id] >= m_assets.size())
    {
        spdlog::error("AssetRegistry::DestroyAsset: Invalid asset ID.");return;
    }

    uint32_t index = it->second;

    if (m_assets[index]->IsLoaded())
    {
        // Unload
    }

    m_assets[index] = nullptr;
    m_assetsById.erase(it);
    m_freeIndex.push_back(index);

    spdlog::info("AssetRegistry::DestroyAsset: Asset ID {} destroyed.", id);
}

bool AssetRegistry::LoadAsset(uint32_t id)
{
    auto it = m_assetsById.find(id);
    if (it == m_assetsById.end())
    {
        return false;
    }

    Core::KSharedPtr<Asset>& asset = m_assets[it->second];
    if (!asset || asset->IsLoaded())
    {
        return true;
    }

    std::string filePath = CacheAssetPath(asset->m_name.c_str(), ASSET_EXTENSION);
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open())
    {
        spdlog::error("AssetRegistry::LoadAsset: Impossible d'ouvrir le fichier {}", filePath);
        return false;
    }

    if (asset->Load(file))
    {
        asset->isLoaded = true;
        spdlog::info("Asset '{}' (ID: {}) loaded.", asset->GetName(), id);
        return true;
    }

    return false;
}

void AssetRegistry::UnloadAsset(uint32_t id)
{
    auto it = m_assetsById.find(id);
    if (it == m_assetsById.end()) return;

    Core::KSharedPtr<Asset>& asset = m_assets[it->second];
    if (asset && asset->IsLoaded())
    {
        asset->UnLoad();
        asset->isLoaded = false;
        spdlog::info("Asset '{}' (ID: {}) unloaded.", asset->GetName(), id);
    }
}

END_NAMESPACE_ASSETMANAGER
