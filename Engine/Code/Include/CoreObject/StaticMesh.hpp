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
    KAPI virtual ~StaticMesh() = default;

    KAPI static AssetManager::AssetType GetStaticType()
    {
        return AssetManager::AssetType::StaticMesh;
    }
};

END_NAMESPACE_COREOBJECT
