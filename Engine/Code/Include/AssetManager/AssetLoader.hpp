#pragma once

#include "AssetManagerModule.hpp"

#include "ThreadPool.hpp"

BEGIN_NAMESPACE_ASSETMANAGER
    class AssetLoader
{
public:
    AssetLoader();
    ~AssetLoader();

private:

// Deleted constructors & operator
private:
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;
};
END_NAMESPACE_ASSETMANAGER

