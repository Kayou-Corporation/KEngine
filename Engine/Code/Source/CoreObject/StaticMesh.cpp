#include "StaticMesh.hpp"

BEGIN_NAMESPACE_COREOBJECT

StaticMesh::StaticMesh(const AssetManager::AssetType type, const std::string &name, const uint32_t id,uint32_t sourceHash, uint32_t sourceSize)
    : MeshBase(type, name, id, sourceHash, sourceSize)
{

}

bool StaticMesh::Load(std::ifstream &file)
{
    (void)file;
    return true;
}

void StaticMesh::UnLoad()
{

}

END_NAMESPACE_COREOBJECT
