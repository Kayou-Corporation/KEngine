//
// Created by eliot on 23/09/2026.
//

#ifndef KENGINE_STATICMESH_HPP
#define KENGINE_STATICMESH_HPP
#include "MeshBase.hpp"

#endif //KENGINE_STATICMESH_HPP

BEGIN_NAMESPACE_COREOBJECT

class StaticMesh : public MeshBase
{
public:
    KAPI StaticMesh(const AssetManager::AssetType type, const std::string& name, const uint32_t id, uint32_t sourceHash, uint32_t sourceSize);
    KAPI virtual ~StaticMesh() = default;

    KAPI static AssetManager::AssetType GetStaticType()
    {
        return AssetManager::AssetType::StaticMesh;
    }

protected:
    KAPI bool Load(std::ifstream& file) override;
    KAPI void UnLoad() override;
};

END_NAMESPACE_COREOBJECT
