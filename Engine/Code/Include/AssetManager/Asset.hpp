#pragma once

#include  "Utils/Memory.hpp"
#include "AssetManagerModule.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

enum class AssetType
{
    StaticMesh = 0,
    Texture = 1,

    COUNT = 2
};

class Asset : Core::IResource
{
public:
    //Asset(AssetType type);

protected:
    AssetType m_type;

private:
    void GuessAssetTypeFromSource(const std::string& source);
};

template<typename T>
class SoftAssetPtr
{
public:

private:
    SoftAssetPtr() = delete;
};

END_NAMESPACE_ASSETMANAGER

