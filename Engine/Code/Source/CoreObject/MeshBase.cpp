#include "MeshBase.hpp"

BEGIN_NAMESPACE_COREOBJECT
SubMesh MeshBase::GetSubmeshInfo(uint32_t index) const
{
    if (index >= m_submeshesCount)
    {
        spdlog::error("MeshBase::GetSubmeshInfo: Index {} is out of bounds. Submesh count: {}", index, m_submeshesCount);
        return SubMesh{};
    }

    return m_submeshesInfo[index];
}

std::vector<Vertex> MeshBase::GetSubmeshVertices(uint32_t index) const
{
    if (index >= m_submeshesCount)
    {
        spdlog::error("MeshBase::GetSubmeshVertices: Index {} is out of bounds. Submesh count: {}", index, m_submeshesCount);
        return {};
    }

    const SubMesh& submeshInfo = m_submeshesInfo[index];

    return std::vector<Vertex>(m_vertices.begin() + submeshInfo.verticesOffset, m_vertices.begin() + submeshInfo.verticesOffset + submeshInfo.verticesCount);
}

std::vector<uint32_t> MeshBase::GetSubmeshIndices(uint32_t index) const
{
    if (index >= m_submeshesCount)
    {
        spdlog::error("MeshBase::GetSubmeshIndices: Index {} is out of bounds. Submesh count: {}", index, m_submeshesCount);
        return {};
    }

    const SubMesh& submeshInfo = m_submeshesInfo[index];

    return std::vector<uint32_t>(m_indices.begin() + submeshInfo.indicesOffset, m_indices.begin() + submeshInfo.indicesOffset + submeshInfo.indicesCount);
}

END_NAMESPACE_COREOBJECT
