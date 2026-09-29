#include  "AssetRegistry.hpp"

#include <fstream>
#include <filesystem>

#include "AssetLoader.hpp"
#include  "Utils/File.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

template<>
void AssetRegistry::WriteAssetRawData<RawStaticMeshData>(std::ofstream& file, const RawStaticMeshData& data)
{
    const std::vector<CoreObject::Vertex>& vertices = data.vertices;
    Core::Write(file, static_cast<uint32_t>(vertices.size()));
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        Core::Write(file, vertices[i].pos.x);
        Core::Write(file, vertices[i].pos.y);
        Core::Write(file, vertices[i].pos.z);

        Core::Write(file, vertices[i].normal.x);
        Core::Write(file, vertices[i].normal.y);
        Core::Write(file, vertices[i].normal.z);

        Core::Write(file, vertices[i].uv.x);
        Core::Write(file, vertices[i].uv.y);
    }

    const std::vector<uint32_t>& indices = data.indices;
    Core::Write(file, static_cast<uint32_t>(indices.size()));
    for (uint32_t i = 0; i < indices.size(); ++i)
    {
        Core::Write(file, indices[i]);
    }

    const std::vector<CoreObject::SubMesh>& submeshes = data.submeshes;
    Core::Write(file, static_cast<uint32_t>(submeshes.size()));
    for (uint32_t i = 0; i < submeshes.size(); ++i)
    {
        Core::Write(file, submeshes[i]);
    }
}

template<>
RawStaticMeshData AssetRegistry::ReadAssetRawData<RawStaticMeshData>(std::ifstream& file)
{
    // Skip first part of the files to get the data.
    size_t commonDataSize;
    Core::Read(file, commonDataSize);
    file.seekg(commonDataSize, std::ios::beg);

    RawStaticMeshData data;

    uint32_t verticesCount;
    Core::Read(file, verticesCount);
    data.vertices.resize(verticesCount);
    for (uint32_t i = 0; i < verticesCount; ++i)
    {
        CoreObject::Vertex vertex;
        Core::Read(file, vertex.pos.x);
        Core::Read(file, vertex.pos.y);
        Core::Read(file, vertex.pos.z);

        Core::Read(file, vertex.normal.x);
        Core::Read(file, vertex.normal.y);
        Core::Read(file, vertex.normal.z);

        Core::Read(file, vertex.uv.x);
        Core::Read(file, vertex.uv.y);

        data.vertices.push_back(vertex);
    }

    uint32_t indices;
    Core::Read(file, indices);
    data.indices.resize(indices);
    for (uint32_t i = 0; i < indices; ++i)
    {
        uint32_t indice;
        Core::Read(file, indice);
        data.indices.push_back(indice);
    }

    uint32_t submeshes;
    Core::Read(file, submeshes);
    data.submeshes.resize(submeshes);
    for (uint32_t i = 0; i < submeshes; ++i)
    {
       CoreObject::SubMesh submesh;
       Core::Read(file, submesh);
       data.submeshes.push_back(submesh);
    }

    return data;
}

//template<>
//void AssetRegistry::WriteAssetRawData<RawTextureData>(std::ofstream& file, const RawTextureData& data)
//{
//    // TODO : Implement this
//    file.write(reinterpret_cast<const char*>(&data), 12);
//}

AssetRegistry::AssetRegistry(const std::string_view &folderPath)
{
    m_folderPath = folderPath;
    //m_assets = std::vector<Asset>(100);
}


// TODO : Implement this
void AssetRegistry::RegisterAllAssets()
{

}

void AssetRegistry::CreateAsset(const std::string& assetPath, const std::string& assetName)
{
    // Create only 1 time, become void if called multiple times
    std::filesystem::create_directories("Cache/Assets/");

    // TODO : Maybe add security here to check if already exist ?

    // Actual asset created
    const std::string fullAssetPath = CacheAssetPath(assetName, ASSET_EXTENSION);
    std::ofstream asset(fullAssetPath, std::ios::binary);


    AssetType type = AssetLoader::GuessAssetTypeFromSource(assetPath);
    // Generic data
    size_t commonDataSize = sizeof(uint32_t) + sizeof(AssetType) + sizeof(uint32_t) + (sizeof(char) * assetName.length());
    Core::Write(asset, commonDataSize);

    Core::Write(asset, m_lastAssetId);

    Core::Write(asset, type);

    Core::Write(asset, assetName.length());

    for (uint32_t i = 0; i < assetName.length(); ++i)
    {
        Core::Write(asset, assetName[i]);
    }

    switch (type)
    {
        case AssetType::StaticMesh:
        {
            // TODO: Implement for static mesh
            RawStaticMeshData assetData = AssetLoader::LoadAssetFromSource<RawStaticMeshData>(assetPath);
            WriteAssetRawData(asset, assetData);
            spdlog::info("AssetRegistry::CreateAsset: Creating StaticMesh asset from path: {}", assetPath);

            break;
        }
        case AssetType::Texture:
        {
            // TODO: Implement for texture
            RawTextureData textureData = AssetLoader::LoadAssetFromSource<RawTextureData>(assetPath);
            //WriteAssetRawData<RawTextureData>(asset, textureData);
            spdlog::info("AssetRegistry::CreateAsset: Creating Texture asset from path: {}", assetPath);
            break;
        }
        default:
            spdlog::error("AssetRegistry::CreateAsset: Unsupported asset type for path: {}", assetPath);
            return;
    }

    // Create Asset in memory :
    Core::KUniquePtr<AssetManager::Asset> assetPtr = Core::CreateUniquePtr<AssetManager::Asset>(type, assetName, m_lastAssetId);
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

    Core::KUniquePtr<AssetManager::Asset> assetPtr = Core::CreateUniquePtr<AssetManager::Asset>(assetType, assetName, assetId);
    m_assets.push_back(std::move(assetPtr));
}

std::string AssetRegistry::CacheAssetPath(const std::string &name, const char *ext)
{
    return "Cache/Assets/" + name + ext;
}

void AssetRegistry::DestroyAsset(uint32_t id)
{
    (void)id;
}

END_NAMESPACE_ASSETMANAGER
