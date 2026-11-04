#include "MeshBase.hpp"

#include "ExternalAssetLoader.hpp"
#include "InternalAssetSerializer.hpp"
#include "KEngine.hpp"
#include "RHI/API/CommandList.hpp"
#include "RHI/API/Syncronisation.hpp"

BEGIN_NAMESPACE_COREOBJECT
    MeshBase::MeshBase(const AssetManager::AssetType type, const std::string& name, const uint32_t id, uint32_t sourceHash, uint32_t sourceSize)
    : Asset(type, name, id, sourceHash, sourceSize)
{

}

SubMesh MeshBase::GetSubmeshInfo(uint32_t index) const
{
    if (index >= m_submeshesCount)
    {
        spdlog::error("MeshBase::GetSubmeshInfo: Index {} is out of bounds. Submesh count: {}", index, m_submeshesCount);
        return SubMesh{};
    }

    return m_submeshesInfo[index];
}

bool MeshBase::Load(std::ifstream &file)
{
    if (isLoaded)
    {
        return true;
    }

    AssetManager::RawMeshData rawMeshData = AssetManager::InternalAssetSerializer::ReadAssetRawData<AssetManager::RawMeshData>(file);

    // Only local, will get empty
    std::vector<Vertex> vertices = rawMeshData.vertices;
    std::vector<uint32_t> indices = rawMeshData.indices;

    // Read the raw mesh data
    m_verticesCount = vertices.size();
    m_indicesCount = indices.size();
    m_submeshesInfo = rawMeshData.submeshes;
    m_submeshesCount = m_submeshesInfo.size();

    // local device reference
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();
    Core::CommandListHandle uploadDataGpuCmdList = device->GetCommandList(Core::QueueType::Graphics);
    uploadDataGpuCmdList->Open();

    // vertex buffer creation
    uint32_t vertexBufferSize = static_cast<uint32_t>(vertices.size()) * sizeof(Vertex);
    if (m_vertexBuffer == nullptr)
    {
        CreateVertexBuffer(device, vertexBufferSize);
    }

    uploadDataGpuCmdList->SetBufferData(m_vertexBuffer, vertices.data(), vertexBufferSize, 0);

    // index buffer creation
    uint32_t indexBufferSize = static_cast<uint32_t>(indices.size()) * sizeof(uint32_t);
    if (m_indexBuffer == nullptr)
    {
        CreateIndexBuffer(device, indexBufferSize);
    }
    uploadDataGpuCmdList->SetBufferData(m_indexBuffer, indices.data(), indexBufferSize, 0);

    uploadDataGpuCmdList->Close();

    Core::SubmitInfo submitInfo;
    submitInfo.stage = Core::PipelineStage::Transfer;
    device->SubmitCommandList(uploadDataGpuCmdList, submitInfo);

    return true;
}

void MeshBase::UnLoad()
{
    if (!isLoaded)
    {
        return;
    }

    // local device reference
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    if (m_vertexBuffer != nullptr)
    {
        device->DestroyBuffer(m_vertexBuffer);
        m_vertexBuffer = nullptr;
    }

    if (m_indexBuffer != nullptr)
    {
        device->DestroyBuffer(m_indexBuffer);
        m_indexBuffer = nullptr;
    }

    m_verticesCount = 0;
    m_indicesCount = 0;
    m_submeshesInfo.clear();
    m_submeshesCount = 0;

    isLoaded = false;
}

void MeshBase::CreateVertexBuffer(Core::DeviceHandle localDeviceReference, uint32_t bufferSize)
{
    Core::BufferSpecs vertexbufferSpecs{};
    vertexbufferSpecs.primaryUsage = Core::BufferUsage::Vertex;
    vertexbufferSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    vertexbufferSpecs.size = bufferSize;
    vertexbufferSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    vertexbufferSpecs.pipelineStage = Core::PipelineStage::VertexInput;

    m_vertexBuffer = localDeviceReference->CreateBuffer(vertexbufferSpecs);
    hasBeenLoaded = true;
}

void MeshBase::CreateIndexBuffer(Core::DeviceHandle localDeviceReference, uint32_t bufferSize)
{
    Core::BufferSpecs indexBufferSpecs{};
    indexBufferSpecs.primaryUsage = Core::BufferUsage::Index;
    indexBufferSpecs.additionalUsages = { Core::BufferUsage::TransferDst };
    indexBufferSpecs.size = bufferSize;
    indexBufferSpecs.memoryAccess = Core::MemoryAccess::GpuOnly;
    indexBufferSpecs.pipelineStage = Core::PipelineStage::VertexInput;

    m_indexBuffer = localDeviceReference->CreateBuffer(indexBufferSpecs);
    hasBeenLoaded = true;
}

END_NAMESPACE_COREOBJECT
