#pragma once

#include "AssetManagerModule.hpp"

#include "Asset.hpp"
#include  "AssetLoader.hpp"
#include "AssetRegistry.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

class AssetManager
{
public:
    void VoidLoadSync(const std::string_view& assetPath);
    void VoidLoadAsync(const std::string_view& assetPath);


private:
    AssetLoader m_assetLoader;
    AssetRegistry m_assetRegistry;

    AssetType GuessAssetTypeFromSource(const std::string_view& source);

// Deleted constructors & operator
private:
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;
    AssetManager& operator=(AssetManager&&) = delete;
};

END_NAMESPACE_ASSETMANAGER
