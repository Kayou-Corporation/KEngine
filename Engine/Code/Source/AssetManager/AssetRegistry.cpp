#include  "AssetRegistry.hpp"

#include <fstream>
#include <filesystem>

#include "ExternalAssetLoader.hpp"
#include "InternalAssetSerializer.hpp"
#include  "Utils/File.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

//template<>
//void AssetRegistry::WriteAssetRawData<RawTextureData>(std::ofstream& file, const RawTextureData& data)
//{
//    // TODO : Implement this
//    file.write(reinterpret_cast<const char*>(&data), 12);
//}

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

}

void AssetRegistry::CreateAsset(const std::string& assetPath, const std::string& assetName, const AssetType type)
{
    // Create only 1 time, become void if called multiple times
    std::filesystem::create_directories(m_folderPath);

    // Actual asset created
    const std::string fullAssetPath = CacheAssetPath(assetName, ASSET_EXTENSION);
    std::ofstream asset(fullAssetPath, std::ios::binary);

    // Type of asset
    AssetType usedAssetType;
    if (type == AssetType::DefaultGuess)
    {
        usedAssetType = ExternalAssetLoader::GuessAssetTypeFromSource(assetPath);
    }
    else
    {
        usedAssetType = type;
    }

    // Write asset header
    size_t commonDataSize = sizeof(uint32_t) + sizeof(AssetType) + sizeof(uint32_t) + (sizeof(char) * assetName.length());
    AssetHeader header =
    {
        .size = commonDataSize,
        .id = m_lastAssetId,
        .type = usedAssetType,
        .name = assetName
    };

    InternalAssetSerializer::WriteAssetHeader(asset, header);

    switch (type)
    {
        case AssetType::StaticMesh:
        {
            // TODO: Implement for static mesh
            RawStaticMeshData assetData = ExternalAssetLoader::LoadAssetFromSource<RawStaticMeshData>(assetPath);
            InternalAssetSerializer::WriteAssetRawData(asset, assetData);
            spdlog::info("AssetRegistry::CreateAsset: Creating StaticMesh asset from path: {}", assetPath);

            break;
        }
        case AssetType::Texture2D:
        {
            // TODO: Implement for texture
            RawTextureData textureData = ExternalAssetLoader::LoadAssetFromSource<RawTextureData>(assetPath);
            InternalAssetSerializer::WriteAssetRawData(asset, textureData);
            spdlog::info("AssetRegistry::CreateAsset: Creating Texture asset from path: {}", assetPath);
            break;
        }
        default:
            spdlog::error("AssetRegistry::CreateAsset: Unsupported asset type for path: {}", assetPath);
            return;
    }

    // Create Asset in memory :
    Core::KSharedPtr<AssetManager::Asset> assetPtr = Core::CreateRefPtr<AssetManager::Asset>(type, assetName, m_lastAssetId);
    m_assets.push_back(std::move(assetPtr));

    m_lastAssetId++;
}

void AssetRegistry::RegisterAsset(const std::string&assetPath)
{
    uint32_t assetId;
    AssetType assetType;
    uint32_t assetNameLength;
    std::string assetName;

    std::ifstream asset(assetPath, std::ios::binary);
    Core::Read(asset, assetId);
    Core::Read(asset, assetType);
    Core::Read(asset, assetNameLength);
    for (uint32_t i = 0; i < assetNameLength; ++i)
    {
        char c;
        Core::Read(asset, c);
        assetName += c;
    }

    Core::KSharedPtr<AssetManager::Asset> assetPtr = Core::CreateRefPtr<AssetManager::Asset>(assetType, assetName, assetId);
    m_assets.push_back(std::move(assetPtr));
}

std::string AssetRegistry::CacheAssetPath(const std::string &name, const char *ext)
{
    return m_folderPath + name + ext;
}

void AssetRegistry::DestroyAsset(uint32_t id)
{
    (void)id;
}

END_NAMESPACE_ASSETMANAGER
