#pragma once

#include <sl/Camera.hpp>

using namespace sl;
using namespace std;

static const string TOGGLE_LASER_CROSSHAIR_NAME = "ToggleLaserCrosshair";
static const string TOGGLE_LASER_PATH_NAME = "ToggleLaserPath";
static const string TOGGLE_GRAVITY_CROSSHAIR_NAME = "ToggleGravityCrosshair";
static const string TOGGLE_GRAVITY_PATH_NAME = "ToggleGravityPath";

//Integers.
static const string SCREEN_ROTATION_NAME = "ScreenRotation";
static const string CAM_RESOLUTION_NAME = "CamResolution";
static const string CAM_PERFORMANCE_MODE_NAME = "CamPerformanceMode";

//Floats. 
static const string FORWARD_SPEED_NAME = "ForwardSpeedMPS";
static const string HOP_UP_SPEED_NAME = "HopUpSpeedMPS";
static const string CAM_X_POS_NAME = "CamXPos";
static const string CAM_Y_POS_NAME = "CamYPos";
static const string CAM_Z_POS_NAME = "CamZPos";
static const string CAM_X_ROT_NAME = "CamXRotDegrees";
static const string CAM_Y_ROT_NAME = "CamYRotDegrees";
static const string CAM_Z_ROT_NAME = "CamZRotDegrees";

class Config
{
public: 
	static string ListAllSettings();

#pragma region SettingsProperties
	//BOOL
	static inline const bool& toggleLaserCrosshair() 
	{
		return GetBoolSetting(TOGGLE_LASER_CROSSHAIR_NAME);
	}
	static inline const void toggleLaserCrosshair(bool newvalue)
	{
		SetBoolSetting(TOGGLE_LASER_CROSSHAIR_NAME, newvalue);
	}

	static inline const bool& toggleLaserPath()
	{
		return GetBoolSetting(TOGGLE_LASER_PATH_NAME);
	}
	static inline const void toggleLaserPath(bool newvalue)
	{
		SetBoolSetting(TOGGLE_LASER_PATH_NAME, newvalue);
	}

	static inline const bool& toggleGravityCrosshair()
	{
		return GetBoolSetting(TOGGLE_GRAVITY_CROSSHAIR_NAME);
	}
	static inline const void toggleGravityCrosshair(bool newvalue)
	{
		SetBoolSetting(TOGGLE_GRAVITY_CROSSHAIR_NAME, newvalue);
	}

	static inline const bool& toggleGravityPath()
	{
		return GetBoolSetting(TOGGLE_GRAVITY_PATH_NAME);
	}
	static inline const void toggleGravityPath(bool newvalue)
	{
		SetBoolSetting(TOGGLE_GRAVITY_PATH_NAME, newvalue);
	}

	//INT/ENUM
	static inline const int& screenRotation()
	{
		return GetIntSetting(SCREEN_ROTATION_NAME);
	}
	static inline const void screenRotation(int newvalue)
	{
		SetIntSetting(SCREEN_ROTATION_NAME, newvalue);
	}

	static inline const sl::RESOLUTION& camResolution()
	{
		return (sl::RESOLUTION)GetIntSetting(CAM_RESOLUTION_NAME);
	}
	static inline const void camResolution(sl::RESOLUTION& newvalue)
	{
		SetIntSetting(CAM_RESOLUTION_NAME, (int)newvalue);
	}

	static inline const sl::DEPTH_MODE& camPerformanceMode()
	{
		return (sl::DEPTH_MODE)GetIntSetting(CAM_PERFORMANCE_MODE_NAME);
	}
	static inline const void camPerformanceMode(sl::DEPTH_MODE& newvalue)
	{
		SetIntSetting(CAM_PERFORMANCE_MODE_NAME, (int)newvalue);
	}

	//FLOAT
	static inline const float& forwardSpeedMPS()
	{
		return GetFloatSetting(FORWARD_SPEED_NAME);
	}
	static inline const void forwardSpeedMPS(float newvalue)
	{
		SetFloatSetting(FORWARD_SPEED_NAME, newvalue);
	}

	static inline const float& hopUpSpeedMPS()
	{
		return GetFloatSetting(HOP_UP_SPEED_NAME);
	}
	static inline const void hopUpSpeedMPS(float newvalue)
	{
		SetFloatSetting(HOP_UP_SPEED_NAME, newvalue);
	}

	static inline const float& camXPos()
	{
		return GetFloatSetting(CAM_X_POS_NAME);
	}
	static inline const void camXPos(float newvalue)
	{
		SetFloatSetting(CAM_X_POS_NAME, newvalue);
	}
	static inline const float& camYPos()
	{
		return GetFloatSetting(CAM_Y_POS_NAME);
	}
	static inline const void camYPos(float newvalue)
	{
		SetFloatSetting(CAM_Y_POS_NAME, newvalue);
	}
	static inline const float& camZPos()
	{
		return GetFloatSetting(CAM_Z_POS_NAME);
	}
	static inline const void camZPos(float newvalue)
	{
		SetFloatSetting(CAM_Z_POS_NAME, newvalue);
	}

	static inline const float& camXRot()
	{
		return GetFloatSetting(CAM_X_ROT_NAME);
	}
	static inline const void camXRot(float newvalue)
	{
		SetFloatSetting(CAM_X_ROT_NAME, newvalue);
	}
	static inline const float& camYRot()
	{
		return GetFloatSetting(CAM_Y_ROT_NAME);
	}
	static inline const void camYRot(float newvalue)
	{
		SetFloatSetting(CAM_Y_ROT_NAME, newvalue);
	}
	static inline const float& camZRot()
	{
		return GetFloatSetting(CAM_Z_ROT_NAME);
	}
	static inline const void camZRot(float newvalue)
	{
		SetFloatSetting(CAM_Z_ROT_NAME, newvalue);
	}

	

#pragma endregion

private:

	static bool LoadSettingsFromDisk(string filename); //TODO: Make private. 
	static bool SaveSettingsToDisk(string filename); //TODO: Make private. 


	static bool GetBoolSetting(string settingname); //TODO: Make private. 
	static void SetBoolSetting(string settingname, bool value); //TODO: Make private. 

	static int GetIntSetting(string settingname);
	static void SetIntSetting(string settingname, int value);

	static float GetFloatSetting(string settingname);
	static void SetFloatSetting(string settingname, float value);

	//Constant string names for values. 
	//Booleans.
	//const string TOGGLE_LASER_CROSSHAIR_NAME = "ToggleLaserCrosshair";
	
};