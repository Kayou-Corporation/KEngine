#include "InternalAssetSerializer.hpp"

#include "ExternalAssetLoader.hpp"
#include "Utils/File.hpp"

BEGIN_NAMESPACE_ASSETMANAGER

// ----- Common Data Write / Read ------ //
void InternalAssetSerializer::WriteAssetHeader(std::ofstream& asset, const AssetHeader& headerInfo)
{
    Core::Write(asset, headerInfo.size);

    Core::Write(asset, headerInfo.id);

    Core::Write(asset, headerInfo.type);

    uint32_t nameLength = headerInfo.name.length();
    Core::Write(asset, nameLength);

    for (uint32_t i = 0; i < headerInfo.name.length(); ++i)
    {
        Core::Write(asset, headerInfo.name[i]);
    }
}

AssetHeader InternalAssetSerializer::ReadAssetHeader(std::ifstream& asset)
{
    uint32_t headerSize;
    Core::Read(asset, headerSize);

    uint32_t assetId;
    Core::Read(asset, assetId);

    AssetType assetType;
    Core::Read(asset, assetType);


    uint32_t assetNameLength;
    Core::Read(asset, assetNameLength);

    std::string assetName;
    assetName.resize(assetNameLength);
    for (uint32_t i = 0; i < assetNameLength; ++i)
    {
        char c;
        Core::Read(asset, c);
        assetName[i] = c;
    }

    //spdlog::info("Asset name: {}", assetName);
    return AssetHeader
    {
        .size = headerSize,
        .id = assetId,
        .type = assetType,
        .name = assetName
    };
}

// ----- StaticMesh Write / Read ------ //
template<>
void InternalAssetSerializer::WriteAssetRawData<RawMeshData>(std::ofstream& file, const RawMeshData& data)
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
RawMeshData InternalAssetSerializer::ReadAssetRawData<RawMeshData>(std::ifstream& file)
{
    // Skip first part of the files to get the data.
    uint32_t commonDataSize;
    Core::Read(file, commonDataSize);
    file.seekg(commonDataSize, std::ios::beg);

    RawMeshData data;

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

        data.vertices[i] = vertex;
    }

    uint32_t indices;
    Core::Read(file, indices);
    data.indices.resize(indices);
    for (uint32_t i = 0; i < indices; ++i)
    {
        uint32_t indice;
        Core::Read(file, indice);
        data.indices[i] = indice;
    }

    uint32_t submeshes;
    Core::Read(file, submeshes);
    data.submeshes.resize(submeshes);
    for (uint32_t i = 0; i < submeshes; ++i)
    {
        CoreObject::SubMesh submesh;
        Core::Read(file, submesh);
        data.submeshes[i] = submesh;
    }

    return data;
}

// ----- Texture2D Write / Read ------ //
template<>
void InternalAssetSerializer::WriteAssetRawData<RawTextureData>(std::ofstream& file, const RawTextureData& data)
{
    Core::Write(file, data.width);
    Core::Write(file, data.height);
    Core::Write(file, data.channels);

    Core::Write(file, static_cast<uint32_t>(data.pixels.size()));
    for (uint32_t i = 0; i < data.pixels.size(); ++i)
    {
        Core::Write(file, data.pixels[i]);
    }
}

template<>
RawTextureData InternalAssetSerializer::ReadAssetRawData<RawTextureData>(std::ifstream& file)
{
    // Skip first part of the files to get the data.
    uint32_t commonDataSize;
    Core::Read(file, commonDataSize);
    file.seekg(commonDataSize, std::ios::beg);

    RawTextureData data;
    Core::Read(file, data.width);
    Core::Read(file, data.height);
    Core::Read(file, data.channels);

    data.pixels.resize(data.width * data.height * 4);
    for (uint32_t i = 0; i < data.pixels.size(); ++i)
    {
        Core::Read(file, data.pixels[i]);
    }

    return data;
}

END_NAMESPACE_ASSETMANAGER