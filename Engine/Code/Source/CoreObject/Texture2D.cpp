#include "Texture2D.hpp"

#include "ExternalAssetLoader.hpp"
#include "InternalAssetSerializer.hpp"
#include "KEngine.hpp"
#include "RHI/API/Device.hpp"
#include "RHI/API/CommandList.hpp"
#include "RHI/API/Syncronisation.hpp"
#include "spdlog/fmt/bundled/compile.h"

BEGIN_NAMESPACE_COREOBJECT
    Texture2D::Texture2D(const AssetManager::AssetType type, const std::string &name, const uint32_t id, uint32_t sourceHash, uint32_t sourceSize)
    : Asset(type, name, id, sourceHash, sourceSize)
{

}

Texture2D::~Texture2D()
{
    UnLoad();
}

bool Texture2D::Load(std::ifstream &file)
{
    if (isLoaded)
    {
        return true;
    }

    AssetManager::RawTextureData rawTextureData = AssetManager::InternalAssetSerializer::ReadAssetRawData<AssetManager::RawTextureData>(file);

    m_width = rawTextureData.width;
    m_height = rawTextureData.height;
    m_channelsCount = rawTextureData.channels;

    std::vector<uint8_t>& pixelData = rawTextureData.pixels;

    // local device reference
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();
    Core::CommandListHandle uploadDataGpuCmdList = device->GetCommandList(Core::QueueType::Graphics);
    uploadDataGpuCmdList->Open();

    if (m_image == nullptr)
    {
        Core::ImageSpecs textureImageSpecs;
        textureImageSpecs.format = Core::Format::RGBA8_SRGB;
        textureImageSpecs.targetLayout = Core::Layout::ShaderReadOnly;
        textureImageSpecs.type = Core::ImageType::Image2D;
        textureImageSpecs.viewType = Core::ImageViewType::Image2D;
        textureImageSpecs.viewAspect = Core::ImageViewAspect::Color;
        textureImageSpecs.usages = { Core::ImageUsage::TransferDst, Core::ImageUsage::ShaderSampled };
        textureImageSpecs.extent = { m_width, m_height, 1 };

        m_image = device->CreateImage(textureImageSpecs);
    }

    uploadDataGpuCmdList->SetImageData(m_image, pixelData.data(), static_cast<uint32_t>(pixelData.size()));
    uploadDataGpuCmdList->TransitionImageLayout(m_image,Core::Layout::ShaderReadOnly);

    uploadDataGpuCmdList->Close();

    Core::SubmitInfo submitInfo;
    submitInfo.stage = Core::PipelineStage::Transfer;
    device->SubmitCommandList(uploadDataGpuCmdList, submitInfo);

    return true;
}

void Texture2D::UnLoad()
{
    if (!isLoaded)
    {
        return;
    }

    // local device reference
    Core::DeviceHandle device = Core::KEngine::Get().GetDevice();

    if (m_image != nullptr)
    {
        device->DestroyImage(m_image);
        m_image = nullptr;
    }
}

END_NAMESPACE_COREOBJECT
