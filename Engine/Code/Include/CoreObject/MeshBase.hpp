#pragma once

#include "CoreObjectModule.hpp"

#include "Asset.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

BEGIN_NAMESPACE_COREOBJECT

// TODO : Maybe move this struct to the Core ?
struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct SubMesh
{
    uint32_t verticesOffset = 0;
    uint32_t verticesCount = 0;
    uint32_t indicesOffset = 0;
    uint32_t indicesCount = 0;
};

class MeshBase : public AssetManager::Asset
{
public:
    KAPI MeshBase(const AssetManager::AssetType type, const std::string& name, const uint32_t id, uint32_t sourceHash, uint32_t sourceSize);
    KAPI virtual ~MeshBase() = default;

    // Base utility
    KAPI const std::vector<Vertex>& GetMesh() const { return m_vertices; }
    KAPI const std::vector<uint32_t>& GetIndices() const { return m_indices; }
    KAPI const std::vector<SubMesh>& GetSubmeshesInfo() const { return m_submeshesInfo; }
    KAPI SubMesh GetSubmeshInfo(uint32_t index) const;
    KAPI uint32_t GetSubmeshesCount() const { return m_submeshesCount; }

    // Get submeshes infos
    KAPI std::vector<Vertex> GetSubmeshVertices(uint32_t index) const;
    KAPI std::vector<uint32_t> GetSubmeshIndices(uint32_t index) const;



protected:
    KAPI bool Load(std::ifstream& file) override;
    KAPI void UnLoad() override;

    // Raw Data
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    // Infos
    std::vector<SubMesh> m_submeshesInfo;
    uint32_t m_submeshesCount = 0;
};

END_NAMESPACE_COREOBJECT
