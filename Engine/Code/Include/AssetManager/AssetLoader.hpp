#pragma once

#include "Asset.hpp"
#include "AssetManagerModule.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_ASSETMANAGER
class AssetLoader
{
public:
    AssetLoader() = delete;
    ~AssetLoader() = delete;

    AssetLoader(const AssetLoader&) = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;

public:
    static AssetType GuessAssetTypeFromSource(const std::string_view& path);
};
END_NAMESPACE_ASSETMANAGER

