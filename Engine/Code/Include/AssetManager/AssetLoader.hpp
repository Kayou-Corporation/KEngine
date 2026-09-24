#pragma once

#include "Asset.hpp"
#include "AssetManagerModule.hpp"

#include "ThreadPool.hpp"
#include "Utils/Memory.hpp"
#include "MeshBase.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

struct RawStaticMeshData
{
    std::vector<CoreObject::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<CoreObject::SubMesh> submeshes;
};

struct RawTextureData
{
    std::vector<uint8_t> pixels;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};

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
    template<typename RawData>
    static RawData LoadAssetFromSource(const std::string_view& path);

    static AssetType GuessAssetTypeFromSource(const std::string_view& path);
};
END_NAMESPACE_ASSETMANAGER

