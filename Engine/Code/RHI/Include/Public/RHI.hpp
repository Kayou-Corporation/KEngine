#pragma once
#include "Utils/Core.hpp"
#include "Utils/Export.hpp"
#include "Utils/Memory.hpp"

#define RHI_NAMESPACE RHI

#define BEGIN_NAMESPACE_RHI BEGIN_NAMESPACE_KAYOU namespace RHI_NAMESPACE {
#define END_NAMESPACE_RHI } /* namespace RHI_NAMESPACE */ END_NAMESPACE_KAYOU

BEGIN_NAMESPACE_RHI

// ---------- FORWARD DECLARATIONS ----------
class Buffer;
class CommandList;
class Image;
class Instance;
class Device;
struct RenderingInfo;

class Fence;
class Semaphore;

class Pipeline;
class GraphicsPipeline;

class PipelineLayout;
class DescriptorSet;

class Surface;

class Swapchain;
struct SwapchainSpecs;

struct BufferSpecs;

struct ImageSpecs;
struct SwapchainImageSpecs;

struct SemaphoreSpecs;
struct SubmitInfo;

struct GraphicsPipelineSpecs;

class DescriptorSetLayout;
class PushConstantLayout;

class Sampler;
struct SamplerSpecs;

class Shader;

class Window;

struct SurfaceSpecs;

struct DeviceSpecs;

class RenderPass;

// ---------- HANDLE TYPEDEFS ----------
typedef Core::RefCountPtr<Buffer> BufferHandle;
typedef Core::RefCountPtr<CommandList> CommandListHandle;
typedef Core::RefCountPtr<DescriptorSet> DescriptorSetHandle;
typedef Core::RefCountPtr<DescriptorSetLayout> DescriptorSetLayoutHandle;
typedef Core::RefCountPtr<Device> DeviceHandle;
typedef Core::RefCountPtr<Fence> FenceHandle;
typedef Core::RefCountPtr<GraphicsPipeline> GraphicsPipelineHandle;
typedef Core::RefCountPtr<Instance> InstanceHandle;
typedef Core::RefCountPtr<Pipeline> PipelineHandle;
typedef Core::RefCountPtr<PipelineLayout> PipelineLayoutHandle;
typedef Core::RefCountPtr<PushConstantLayout> PushConstantLayoutHandle;
typedef Core::RefCountPtr<RenderPass> RenderPassLayout;
typedef Core::RefCountPtr<Semaphore> SemaphoreHandle;
typedef Core::RefCountPtr<Sampler> SamplerHandle;
typedef Core::RefCountPtr<Shader> ShaderHandle;

// ---------- INSTANCE RELATED ----------
struct Version
{
	Version() = default;
	Version(int _major, int _minor, int _patch)
		: major(_major), minor(_minor), patch(_patch) {};

	~Version() = default;

	int major = 0;
	int minor = 0;
	int patch = 1;
};

enum class DebugLayers 
{
	Validation,
	DeviceMonitor,
	DeviceSimulation,
	APIDump
};

// ---------- DEVICE RELATED ----------
enum class GpuType
{
	Discrete, // Classic gpu 
	Virtual, // Virtual
	Cpu // Cpu
};

enum class Extensions
{
	Swapchain, // Check if rendering is supported

	DynamicRendering, // DynamicRendering
	ShaderObject, // Shader not bound to pipeline

	ExtendedDynamicState,
	ExtendedDynamicState2
	// Add other extensions ? depend on driver
};

enum class Features
{
	GeometryShader,
	TesselationShader
	// Geometry & tesselation ?
};

//-------- QUEUE / COMMAND -----------
enum class QueueType
{
	Graphics,
	Transfer,
	Compute
};

// IMAGE / PRESENTATION RELATED
enum class Format
{
	BGRA8_SRGB, // Basic swapchain format
	
	RGBA8_SRGB,  //  
	RGB8_SRGB,   // Some basic color formats
	RGBA8_UNORM, // 
	RGB8_UNORM,  //     
	 
	D32_SFLOAT,         //
	D32_SFLOAT_S8_UINT, // Some basic depth formats
	D24_UNORM_S8_UINT,  // 

	Undefined
};

enum class ShaderDataType
{
	Float32_1,     // float
	Float32_2,     // vec2
	Float32_3,     // vec3
	Float32_4,     // vec4
	Int32_1,       // int
	Int32_2,       // ivec2
	Int32_3,       // ivec3
	Int32_4,       // ivec4
	Uint32_1,      // uint
	Uint32_2,      // uvec2
	Uint32_3,      // uvec3
	Uint32_4,      // uvec4

	Undefined
};

enum class Layout
{
	Present, // basic presentation for swapchain

	ColorAttachment,        // basic color image layout (RenderTarget)
	DepthStencilAttachment, // basic depth image layout (RenderTarget)

	ShaderReadOnly,        // basic depth image layout (Textures)
	DepthStencilReadOnly,  // basic depth image layout (Textures)

	TransferSrc, // Transfer layout (Source for copy)
	TransferDst, // Transfer layout (Destination for copy)

	Undefined
};

enum class SampleCount // Sample count
{
	Count1,
	Count2,
	Count4,
	Count8,
	Count16,
	Count32,
	Count64
};

enum class ImageUsage
{
	ColorAttachment,
	DepthStencilAttachment,

	TransferSrc,
	TransferDst,

	ShaderSampled,
	Storage,

	Undefined
};

enum class ImageType
{
	Image1D,
	Image2D,
	Image3D,

	Undefined
};

enum class ImageViewType
{
	Image1D,
	Image2D,
	Image3D,
	ImageCube,

	Undefined
};

enum class ImageViewAspect
{
	Color,
	Depth,

	Undefined
};

enum class ImageSource
{
	Cpu,
	Gpu,

	Undefined
};

struct Extent2D
{
public:
	Extent2D() = default;
	Extent2D(uint32_t _x, uint32_t _y)
	{
		x = _x;
		y = _y;
	}

	~Extent2D() = default;

	uint32_t x = 0;
	uint32_t y = 0;
};

struct Extent3D
{
public:
	Extent3D() = default;
	Extent3D(uint32_t _x, uint32_t _y, uint32_t _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	~Extent3D() = default;

	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t z = 0;
};

enum class PresentMode
{
	Immediate,     // No vsync, may tear
	Mailbox,       // Low latency, triple buffering style
	Fifo,          // VSync (always supported)
	FifoRelaxed,   // VSync but late frames don't wait

	Undefined
};

// BUFFER
enum class BufferUsage
{
	Vertex,
	Index,

	Storage,

	Uniform,

	TransferSrc,
	TransferDst
};

enum class MemoryAccess
{
	GpuOnly,
	Upload,
	Readback,
	Dynamic
};

enum class PipelineStage
{
	VertexInput,
	VertexShader,
	FragmentShader,
	ComputeShader,
	GeometryShader,
	Transfer,
	ColorOutput,
	None
};

enum class ShaderStage
{
	Vertex,
	Fragment,
	Compute,
	Geometry,
	Tesselation,
	All
};

enum class PipelineType
{
	Graphics,
	Compute
};

enum class PipelineBindPoint
{
	Graphics,
	Compute,
	RayTracing
};

enum class DynamicState
{
	ViewPort,
	Scissor
};

enum class CullMode
{
	Front,
	Back,
	None
};

enum class FrontFace
{
	ClockWise,
	CounterClockWise
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

enum class LoadOp
{
	Load,
	Clear,
	DontCare
};

enum class StoreOp
{
	Store,
	DontCare
};

struct Offset2D
{
	Offset2D() = default;

	Offset2D(int inX, int inY) { x = inX; y = inY; }
	int x = 0;
	int y = 0;
};

	struct ClearValue
	{
		ClearValue() = default;

		ClearValue(float inX, float inY, float inZ, float inA)
		{
			x = inX;
			y = inY;
			z = inZ;
			a = inA;
		};

		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		float a = 0.f;
	};

enum class SemaphoreType
{
	Binary,
	Timeline
};

constexpr const char* ShaderStageToEntry(ShaderStage sType)
{
	switch (sType)
	{
	case ShaderStage::Vertex:
		return "vsMain";
	case ShaderStage::Fragment:
		return "fsMain";
	case ShaderStage::Compute:
		return "csMain";
	case ShaderStage::Geometry:
		return "gsMain";
	case ShaderStage::Tesselation:
		return "tsMain";
		default:
			return "";
	}

	return "main";
}

enum class VertexInputRate : uint8_t
{
	PerVertex,
	PerInstance
};

enum class DescriptorType
{
	Sampler,
	CombinedImageSampler,
	SampledImage,
	StorageImage,
	UniformTexelBuffer,
	StorageTexelBuffer,
	UniformBuffer,
	StorageBuffer,
	UniformBufferDynamic,
	StorageBufferDynamic,
	InputAttachment,
	AccelerationStructure
};

enum class Filter 
{
	Nearest,
	Linear
};

enum class SamplerAddressMode 
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
	MirrorClampToEdge
};

enum class SamplerMipmapMode 
{
	Nearest,
	Linear
};

enum class BorderColor 
{
	FloatTransparentBlack,
	IntTransparentBlack,
	FloatOpaqueBlack,
	IntOpaqueBlack,
	FloatOpaqueWhite,
	IntOpaqueWhite
};

enum class CompareOp 
{
	Never,
	Less,
	Equal,
	LessOrEqual,
	Greater,
	NotEqual,
	GreaterOrEqual,
	Always
};

END_NAMESPACE_RHI