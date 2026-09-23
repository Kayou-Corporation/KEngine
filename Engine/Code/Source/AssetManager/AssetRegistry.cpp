#include  "AssetRegistry.hpp"

#include "AssetLoader.hpp"

BEGIN_NAMESPACE_ASSETMANAGER
    void AssetRegistry::Init()
{
    if (!m_instance)
    {
        m_instance = Core::CreateUniquePtr<AssetRegistry>();
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

AssetRegistry & AssetRegistry::Get()
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
    (void)type;

}

void AssetRegistry::DestroyAsset(uint32_t id)
{
    (void)id;
}

END_NAMESPACE_ASSETMANAGER
