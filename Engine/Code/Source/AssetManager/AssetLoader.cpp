#include  "AssetLoader.hpp"

#ifdef KCOMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmicrosoft-enum-value"
#endif
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#ifdef KCOMPILER_CLANG
#pragma clang diagnostic pop
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BEGIN_NAMESPACE_ASSETMANAGER


AssetType AssetLoader::GuessAssetTypeFromSource(const std::string_view &path)
{
    const std::string_view::size_type dotPos = path.find_last_of('.');
    if (dotPos == std::string_view::npos || dotPos == path.length() - 1)
    {
        spdlog::error("AssetLoader::GuessAssetTypeFromSource: Invalid file path or missing extension: {}", path);
        return AssetType::COUNT;
    }

    std::string filExtension(path.substr(dotPos + 1));

    std::transform(filExtension.begin(), filExtension.end(), filExtension.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (filExtension == "obj" || filExtension == "fbx" || filExtension == "gltf")
    {
        return AssetType::StaticMesh;
    }
    else if (filExtension == "png" || filExtension == "jpg" || filExtension == "jpeg" || filExtension == "bmp")
    {
        return AssetType::Texture;
    }
    else
    {
        spdlog::error("AssetLoader::GuessAssetTypeFromSource: Unsupported file extension: {}", filExtension);
        return AssetType::COUNT;
    }

    return AssetType::COUNT;
}

template<>
RawStaticMeshData AssetLoader::LoadAssetFromSource<RawStaticMeshData>(const std::string_view& path)
{
    RawStaticMeshData rawData;

    Assimp::Importer importer{};
    const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);

    if (!scene || !scene->mRootNode)
    {
        spdlog::error("coudn't find path");
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[i];

        CoreObject::SubMesh submesh;
        submesh.verticesOffset = static_cast<uint32_t>(rawData.vertices.size());
        submesh.verticesCount  = mesh->mNumVertices;
        submesh.indicesOffset  = static_cast<uint32_t>(rawData.indices.size());

        rawData.vertices.reserve(rawData.vertices.size() + mesh->mNumVertices);

        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            CoreObject::Vertex vertex{};

            if (mesh->HasPositions())
            {
                vertex.pos = glm::vec3(
                    mesh->mVertices[v].x,
                    mesh->mVertices[v].y,
                    mesh->mVertices[v].z
                );
            }

            if (mesh->HasNormals())
            {
                vertex.normal = glm::vec3(
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                );
            }

            if (mesh->HasTextureCoords(0))
            {
                vertex.uv = glm::vec2(
                    mesh->mTextureCoords[0][v].x,
                    1.0f - mesh->mTextureCoords[0][v].y
                );
            }

            rawData.vertices.push_back(vertex);
        }

        uint32_t indicesCountBefore = static_cast<uint32_t>(rawData.indices.size());

        for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            const aiFace& face = mesh->mFaces[f];

            for (unsigned int indexIdx = 0; indexIdx < face.mNumIndices; ++indexIdx)
            {
                rawData.indices.push_back(face.mIndices[indexIdx] + submesh.verticesOffset);
            }
        }

        submesh.indicesCount = static_cast<uint32_t>(rawData.indices.size()) - indicesCountBefore;

        rawData.submeshes.push_back(submesh);
    }

    importer.FreeScene();

    return rawData;
}

template<>
RawTextureData AssetLoader::LoadAssetFromSource<RawTextureData>(const std::string_view& path)
{
    RawTextureData rawData;
    int texWidth, texHeight, texChannels;
    stbi_uc* texturePixels = stbi_load(path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!texturePixels)
    {
        spdlog::error("AssetLoader: Failed to load texture at path '{}'. Reason: {}", path, stbi_failure_reason());
        return rawData;
    }

    rawData.pixels.resize(texWidth * texHeight * texChannels);
    memcpy(rawData.pixels.data(), texturePixels, rawData.pixels.size());

    rawData.width = texWidth;
    rawData.height = texHeight;
    rawData.channels = texChannels;

    stbi_image_free(texturePixels);

    return rawData;
}

END_NAMESPACE_ASSETMANAGER
