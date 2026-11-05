#pragma once

#include "CoreObjectModule.hpp"

#include "Asset.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "RHI/API/Buffer.hpp"

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
    KAPI virtual ~MeshBase();

    // Base utility
    KAPI const std::vector<SubMesh>& GetSubmeshesInfo() const { return m_submeshesInfo; }
    KAPI SubMesh GetSubmeshInfo(uint32_t index) const;
    KAPI uint32_t GetSubmeshesCount() const { return m_submeshesCount; }
    KAPI uint32_t GetVerticesCount() const { return m_verticesCount; }
    KAPI uint32_t GetIndicesCount() const { return m_indicesCount; }

    KAPI Core::BufferHandle GetVertexBuffer() const { return m_vertexBuffer; }
    KAPI Core::BufferHandle GetIndexBuffer() const { return m_indexBuffer; }

protected:
    KAPI bool Load(std::ifstream& file) override;
    KAPI void UnLoad() override;

    virtual void CreateVertexBuffer(Core::DeviceHandle localDeviceReference, uint32_t bufferSize);
    virtual void CreateIndexBuffer(Core::DeviceHandle localDeviceReference, uint32_t bufferSize);

    // Data :
    Core::BufferHandle m_vertexBuffer;
    Core::BufferHandle m_indexBuffer;

    // Infos
    std::vector<SubMesh> m_submeshesInfo;
    uint32_t m_submeshesCount = 0;
    uint32_t m_verticesCount = 0;
    uint32_t m_indicesCount = 0;
};

END_NAMESPACE_COREOBJECT
