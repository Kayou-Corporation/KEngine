#pragma once

#include "Asset.hpp"
#include "AssetManagerModule.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_ASSETMANAGER
class AssetLoader
{
public:
    AssetLoader() = default;
    ~AssetLoader() = default;

    //template<typename AssetClass>
    //Core::RefCountPtr<AssetClass> CreateAsset(const std::string_view& assetPath)
    //{

    //}

private:
    AssetType GuessAssetTypeFromSource(const std::string_view& path);

// Deleted constructors & operator
private:
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;
};
END_NAMESPACE_ASSETMANAGER

