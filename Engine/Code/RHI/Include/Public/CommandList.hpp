#pragma once

#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#include "Public/RHI.hpp"

BEGIN_NAMESPACE_RHI

class Buffer;
class Image;
class Device;

class CommandList : virtual public Core::IResource
{
public:
	virtual ~CommandList() = default;

	// base functions to open close commandBuffer
	KENGINE_API virtual void Open() = 0;
	KENGINE_API virtual void Close() = 0;

	KENGINE_API virtual void SetBufferData(Core::RefCountPtr<Buffer> buffer, void* data, uint32_t size, uint32_t offset) = 0;
	KENGINE_API virtual void SetImageData(Core::RefCountPtr<Image> image, void* data, uint32_t size, uint32_t offset) = 0;

	// Rendering
};

END_NAMESPACE_RHI