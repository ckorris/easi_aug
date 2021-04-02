#pragma once

#include <sl/Camera.hpp>
#include <CamUtilities.h>

class IOShortcuts
{
public:
	static void IncrementZoom();
	static void ToggleSimulationOverlay();
	static void IncrementResolution(sl::Camera *zed, RuntimeParameters *runtime_parameters, Resolution *image_size);
	static void SleepMode(sl::Camera &zed);
};