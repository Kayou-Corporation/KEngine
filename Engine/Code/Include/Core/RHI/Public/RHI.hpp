#pragma once


enum RendererAPI
{
	Vulkan = 0
};

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
	ExtendedDynamicState2,
	ExtendedDynamicState3
	// Add other extensions ? depend on driver
};

enum class Features
{
	GeometryShader,
	TesselationShader
	// Geometry & tesselation ?
};

//-------- QUEUE / COMMAND -----------
enum class Queue
{
	Graphics,
	Transfert,
	Compute
};