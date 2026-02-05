#pragma once

enum RendererAPI
{
	Vulkan = 0
};

struct Version
{
	Version() = default;

	Version(int _major, int _minor, int _patch)
		: major(_major), minor(_minor), patch(_patch) {};

	int major = 0;
	int minor = 0;
	int patch = 1;
};

enum class DebugLayers 
{
	GraphicsDebug,
	KhronosValidation,
	DeviceMonitor,
	DeviceSimulation,
	APIDump
};

enum class Extensions
{
	Surface, // Check if instance support rendering
};