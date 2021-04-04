#pragma once

#include <sl/Camera.hpp>
#include <CamUtilities.h>
#include <TextureHolder.h>

class IOShortcuts
{
public:
	static void IncrementZoom();
	static void ToggleSimulationOverlay();
	static void IncrementResolution(sl::Camera *zed, RuntimeParameters *runtime_parameters, Resolution *image_size, TextureHolder *textureHolder);
	static void SleepMode(sl::Camera &zed);
};