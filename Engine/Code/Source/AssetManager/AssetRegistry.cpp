#include  "AssetRegistry.hpp"

#include "AssetLoader.hpp"

BEGIN_NAMESPACE_ASSETMANAGER
AssetRegistry::AssetRegistry(const std::string_view &folderPath)
{
    m_folderPath = folderPath;
}

void AssetRegistry::Init(const std::string_view &folderPath)
{
    if (!m_instance)
    {
        m_instance = Core::CreateUniquePtr<AssetRegistry>(folderPath);
    }
    else
    {
        spdlog::error("AssetRegistry already initialized");
    }
}

void AssetRegistry::Shutdown()
{
    if (!m_instance)
    {
        spdlog::error("AssetRegistry already shut down");
    }
    else
    {
        m_instance.Reset();
    }
}

AssetRegistry& AssetRegistry::Get()
{
    return *m_instance;
}

// TODO : Implement this
void AssetRegistry::RegisterAllAssets()
{

}

void AssetRegistry::CreateAsset(const std::string_view &assetPath)
{
    AssetType type = AssetLoader::GuessAssetTypeFromSource(assetPath);

    switch (type)
    {
        case AssetType::StaticMesh:
        {
            auto assetData = AssetLoader::LoadAssetFromSource<RawStaticMeshData>(assetPath);
            spdlog::info("AssetRegistry::CreateAsset: Creating StaticMesh asset from path: {}", assetPath);

            break;
        }
        case AssetType::Texture:
        {
            // TODO: Implement for texture
            // auto textureData = AssetLoader::LoadAssetFromSource<RawTextureData>(assetPath);
            spdlog::info("AssetRegistry::CreateAsset: Creating Texture asset from path: {}", assetPath);
            break;
        }
        default:
            spdlog::error("AssetRegistry::CreateAsset: Unsupported asset type for path: {}", assetPath);
            return;
    }

}

void AssetRegistry::DestroyAsset(uint32_t id)
{
    (void)id;
}

END_NAMESPACE_ASSETMANAGER
