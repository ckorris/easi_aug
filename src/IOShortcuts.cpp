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

void IOShortcuts::IncrementResolution(sl::Camera *zed, RuntimeParameters *runtime_parameters, Resolution *image_size, TextureHolder *textureHolder)
{
	
	sl::RESOLUTION resolution = Config::camResolution();

	//We use a switch statement here instead of something fancier so that we can avoid 2K, last and invalid indexes.
	switch (resolution)
	{
	case sl::RESOLUTION::VGA:
		Config::camResolution(sl::RESOLUTION::HD720);
		break;
	case sl::RESOLUTION::HD720:
		Config::camResolution(sl::RESOLUTION::HD1080);
		break;
	case sl::RESOLUTION::HD1080:
	default:
		Config::camResolution(sl::RESOLUTION::VGA);
		break;
	}
	
	//TODO: This might break if we're recording an SVO.
	zed->close();

	CamUtilities::InitZed(zed, runtime_parameters, image_size);

	textureHolder->CreateMatrixesFromZed(zed);
}

bool isSleeping = false;
void IOShortcuts::SleepMode(sl::Camera *zed, RuntimeParameters *runtime_parameters,
	Resolution *image_size, TextureHolder *textureHolder, shared_ptr<HotkeyBinding> binding)
{
	if (isSleeping == true)
	{
		isSleeping = false;
		return;
	}

	cout << "Sleep started." << endl;
	zed->close();
	isSleeping = true;
	bool buttonPressed = false;
	sleep_ms(200);
	while (isSleeping == true)
	{
		sleep_ms(20);
		KeyBinding::PreProcess();
		binding->Process();
	}

	cout << "Sleep ended." << endl;

	CamUtilities::InitZed(zed, runtime_parameters, image_size);
	sleep_ms(10);
	textureHolder->CreateMatrixesFromZed(zed);
}