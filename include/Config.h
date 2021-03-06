#pragma once

#include <sl/Camera.hpp>

using namespace sl;
using namespace std;

//Bools. 
static const string TOGGLE_LASER_CROSSHAIR_NAME = "ToggleLaserCrosshair";
static const string TOGGLE_LASER_PATH_NAME = "ToggleLaserPath";
static const string TOGGLE_GRAVITY_CROSSHAIR_NAME = "ToggleGravityCrosshair";
static const string TOGGLE_GRAVITY_PATH_NAME = "ToggleGravityPath";
static const string TOGGLE_DISTANCE_NAME = "ToggleDistance";
static const string TOGGLE_TRAVEL_TIME_NAME = "ToggleTravelTime";

//Integers.
static const string SCREEN_ROTATION_NAME = "ScreenRotation";
static const string IMAGE_ROTATION_NAME = "ImageRotation";
static const string CAM_RESOLUTION_NAME = "CamResolution";
static const string CAM_PERFORMANCE_MODE_NAME = "CamPerformanceMode";
static const string LCD_WIDTH_NAME = "LCDWidth";
static const string LCD_HEIGHT_NAME = "LCDHeight";
static const string ZOOM_LEVEL_NAME = "ZoomLevel";
static const string ZOOM_MAX_LEVEL_NAME = "ZoomMaxLevel";

//Floats. 
static const string FORWARD_SPEED_NAME = "ForwardSpeedMPS";
static const string HOP_UP_RPM_NAME = "HopUpRPM";
static const string CAM_X_POS_NAME = "CamXPos";
static const string CAM_Y_POS_NAME = "CamYPos";
static const string CAM_Z_POS_NAME = "CamZPos";
static const string CAM_X_ROT_NAME = "CamXRotDegrees";
static const string CAM_Y_ROT_NAME = "CamYRotDegrees";
static const string CAM_Z_ROT_NAME = "CamZRotDegrees";
static const string BB_MASS_GRAMS_NAME = "BBMassGrams";
static const string BB_DIAMETER_MM_NAME = "BBDiameterMM";
static const string TEMPERATURE_NAME = "TemperatureC";
static const string RELATIVE_HUMIDITY_NAME = "RelativeHumidity01";
static const string BB_TO_AIR_FRIC_COEF_NAME = "BBToAirFrictionCoefficient";

//Strings.
static const string SVO_RECORD_DIRECTORY = "SVORecordDirectory";

class Config
{
public: 
#pragma region SettingsProperties
	//BOOL
	static inline bool toggleLaserCrosshair() 
	{
		return GetBoolSetting(TOGGLE_LASER_CROSSHAIR_NAME);
	}
	static inline const void toggleLaserCrosshair(bool newvalue)
	{
		SetBoolSetting(TOGGLE_LASER_CROSSHAIR_NAME, newvalue);
	}

	static inline bool toggleLaserPath()
	{
		return GetBoolSetting(TOGGLE_LASER_PATH_NAME);
	}
	static inline const void toggleLaserPath(bool newvalue)
	{
		SetBoolSetting(TOGGLE_LASER_PATH_NAME, newvalue);
	}

	static inline bool toggleGravityCrosshair()
	{
		return GetBoolSetting(TOGGLE_GRAVITY_CROSSHAIR_NAME);
	}
	static inline const void toggleGravityCrosshair(bool newvalue)
	{
		SetBoolSetting(TOGGLE_GRAVITY_CROSSHAIR_NAME, newvalue);
	}

	static inline bool toggleGravityPath()
	{
		return GetBoolSetting(TOGGLE_GRAVITY_PATH_NAME);
	}
	static inline const void toggleGravityPath(bool newvalue)
	{
		SetBoolSetting(TOGGLE_GRAVITY_PATH_NAME, newvalue);
	}

	static inline bool toggleDistance()
	{
		return GetBoolSetting(TOGGLE_DISTANCE_NAME);
	}
	static inline const void toggleDistance(bool newvalue)
	{
		SetBoolSetting(TOGGLE_DISTANCE_NAME, newvalue);
	}

	static inline bool toggleTravelTime()
	{
		return GetBoolSetting(TOGGLE_TRAVEL_TIME_NAME);
	}
	static inline const void toggleTravelTime(bool newvalue)
	{
		SetBoolSetting(TOGGLE_TRAVEL_TIME_NAME, newvalue);
	}


	//INT/ENUM
	static inline int screenRotation()
	{
		return GetIntSetting(SCREEN_ROTATION_NAME);
	}
	static inline const void screenRotation(int newvalue)
	{
		SetIntSetting(SCREEN_ROTATION_NAME, newvalue);
	}

	static inline int imageRotation()
	{
		return GetIntSetting(IMAGE_ROTATION_NAME);
	}
	static inline const void imageRotation(int newvalue)
	{
		SetIntSetting(IMAGE_ROTATION_NAME, newvalue);
	}

	static inline sl::RESOLUTION camResolution()
	{
		return (sl::RESOLUTION)GetIntSetting(CAM_RESOLUTION_NAME);
	}
	static inline const void camResolution(sl::RESOLUTION newvalue)
	{
		SetIntSetting(CAM_RESOLUTION_NAME, (int)newvalue);
	}

	static inline sl::DEPTH_MODE camPerformanceMode()
	{
		return (sl::DEPTH_MODE)GetIntSetting(CAM_PERFORMANCE_MODE_NAME);
	}
	static inline const void camPerformanceMode(sl::DEPTH_MODE newvalue)
	{
		SetIntSetting(CAM_PERFORMANCE_MODE_NAME, (int)newvalue);
	}

	static inline int lcdWidth()
	{
		return GetIntSetting(LCD_WIDTH_NAME);
	}
	static inline const void lcdWidth(int newvalue)
	{
		SetIntSetting(LCD_WIDTH_NAME, newvalue);
	}

	static inline int lcdHeight()
	{
		return GetIntSetting(LCD_HEIGHT_NAME);
	}
	static inline const void lcdHeight(int newvalue)
	{
		SetIntSetting(LCD_HEIGHT_NAME, newvalue);
	}

	static inline int zoomLevel()
	{
		return GetIntSetting(ZOOM_LEVEL_NAME);
	}
	static inline const void zoomLevel(int newvalue)
	{
		SetIntSetting(ZOOM_LEVEL_NAME, newvalue);
	}

	static inline int zoomMaxLevel()
	{
		return GetIntSetting(ZOOM_MAX_LEVEL_NAME);
	}
	static inline const void zoomMaxLevel(int newvalue)
	{
		SetIntSetting(ZOOM_MAX_LEVEL_NAME, newvalue);
	}

	//FLOAT
	static inline float forwardSpeedMPS()
	{
		return GetFloatSetting(FORWARD_SPEED_NAME);
	}
	static inline const void forwardSpeedMPS(float newvalue)
	{
		SetFloatSetting(FORWARD_SPEED_NAME, newvalue);
	}

	static inline float hopUpRPM()
	{
		return GetFloatSetting(HOP_UP_RPM_NAME);
	}
	static inline const void hopUpRPM(float newvalue)
	{
		SetFloatSetting(HOP_UP_RPM_NAME, newvalue);
	}

	static inline float bbMassGrams()
	{
		return GetFloatSetting(BB_MASS_GRAMS_NAME);
	}
	static inline const void bbMassGrams(float newvalue)
	{
		SetFloatSetting(BB_MASS_GRAMS_NAME, newvalue);
	}

	static inline float temperatureC()
	{
		return GetFloatSetting(TEMPERATURE_NAME);
	}
	static inline const void temperatureC(float newvalue)
	{
		SetFloatSetting(TEMPERATURE_NAME, newvalue);
	}

	static inline float relativeHumidity01()
	{
		return GetFloatSetting(RELATIVE_HUMIDITY_NAME);
	}
	static inline const void relativeHumidity01(float newvalue)
	{
		if (newvalue < 0 || newvalue > 1)
		{
			cout << "Warning: Set relativeHumidity01 value to " << newvalue <<
				". Clamping." << endl;
			if (newvalue < 0) newvalue = 0;
			else if (newvalue > 1) newvalue = 1;
		}
		SetFloatSetting(RELATIVE_HUMIDITY_NAME, newvalue);
	}

	static inline float bbDiameterMM()
	{
		return GetFloatSetting(BB_DIAMETER_MM_NAME);
	}
	static inline float bbDiameterMM(float newvalue)
	{
		SetFloatSetting(BB_DIAMETER_MM_NAME, newvalue);
	}

	static inline float camXPos()
	{
		return GetFloatSetting(CAM_X_POS_NAME);
	}
	static inline const void camXPos(float newvalue)
	{
		SetFloatSetting(CAM_X_POS_NAME, newvalue);
	}
	static inline float camYPos()
	{
		return GetFloatSetting(CAM_Y_POS_NAME);
	}
	static inline const void camYPos(float newvalue)
	{
		SetFloatSetting(CAM_Y_POS_NAME, newvalue);
	}
	static inline float camZPos()
	{
		return GetFloatSetting(CAM_Z_POS_NAME);
	}
	static inline const void camZPos(float newvalue)
	{
		SetFloatSetting(CAM_Z_POS_NAME, newvalue);
	}

	static inline float camXRot()
	{
		return GetFloatSetting(CAM_X_ROT_NAME);
	}
	static inline const void camXRot(float newvalue)
	{
		SetFloatSetting(CAM_X_ROT_NAME, newvalue);
	}
	static inline float camYRot()
	{
		return GetFloatSetting(CAM_Y_ROT_NAME);
	}
	static inline const void camYRot(float newvalue)
	{
		SetFloatSetting(CAM_Y_ROT_NAME, newvalue);
	}
	static inline float camZRot()
	{
		return GetFloatSetting(CAM_Z_ROT_NAME);
	}
	static inline const void camZRot(float newvalue)
	{
		SetFloatSetting(CAM_Z_ROT_NAME, newvalue);
	}

	static inline float bbToAirFrictionCoef()
	{
		return GetFloatSetting(BB_TO_AIR_FRIC_COEF_NAME);
	}
	static inline const void bbToAirFrictionCoef(float newvalue)
	{
		SetFloatSetting(BB_TO_AIR_FRIC_COEF_NAME, newvalue);
	}

	//STRING
	static inline string svoRecordDirectory()
	{
		return GetStringSetting(SVO_RECORD_DIRECTORY);
	}
	static inline const void svoRecordDirectory(string newvalue)
	{
		SetStringSetting(SVO_RECORD_DIRECTORY, newvalue);
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


	static string GetStringSetting(string settingname);
	static void SetStringSetting(string settingname, string value);
	
};
