#include <IOShortcuts.h>
#include <Config.h>

void IOShortcuts::IncrementZoom()
{
	int zoom = Config::zoomLevel();
	zoom++;
	if (zoom > Config::zoomMaxLevel())
	{
		zoom = 1;
	}

	Config::zoomLevel(zoom);
}

const int laserMask = 1;
const int gravityMask = 2;

void IOShortcuts::ToggleSimulationOverlay()
{
	int laserFlag = Config::toggleLaserPath();
	int gravityFlag = Config::toggleGravityPath() << 1;

	int combinedValue = laserFlag + gravityFlag;

	combinedValue++;

	if (combinedValue > 3) //laserMask + physicsMask
	{
		combinedValue = 0;
	}

	laserFlag = combinedValue & laserMask;
	gravityFlag = (combinedValue & gravityMask) >> 1;

	Config::toggleLaserPath(laserFlag);
	Config::toggleGravityPath(gravityFlag);

	//While you can technically configure the dots independently of the lines, for the sake of not
	//needing to press a button a billion times for the desired setting, we'll make the dots match the lines.
	Config::toggleLaserCrosshair(laserFlag);
	Config::toggleGravityCrosshair(gravityFlag);
}

