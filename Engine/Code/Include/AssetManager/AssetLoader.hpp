#pragma once

#include "AssetManagerModule.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"

BEGIN_NAMESPACE_ASSETMANAGER
    class AssetLoader
{
public:
    AssetLoader();
    ~AssetLoader();

    //template<typename AssetClass>
    //Core::RefCountPtr<AssetClass> CreateAsset(const std::string_view& assetPath)
    //{

    //}


// Deleted constructors & operator
private:
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;
};
END_NAMESPACE_ASSETMANAGER

