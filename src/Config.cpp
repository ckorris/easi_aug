#include <sl/Camera.hpp>
#include <Config.h>

#include <iostream>
#include <fstream>

using namespace sl;
using namespace std;

const string fileName = "config.txt";

static bool isLoaded;

//Maps for the loaded values. 
static map<string, bool> boolSettings;
static map<string, int> intSettings; //Also holds bools
static map<string, float> floatSettings;

/*
//Bool properties.
static bool toggleLaserCrosshair;
static bool toggleLaserPath;

static bool toggleGravityCrosshair;
static bool toggleGravityPath;

//Int properties.
static int screenRotation; //Increments of 90° clockwise. 
static int camResolution; //sl::RESOLUTION. 0 = 2k, 1 = 1080, 2 = 720, 3 = VGA. 
static int camPerformanceMode; //sl::DEPTH_MODE. 0 - None, 1 = Performance, 2 = Quality, 3 = Ultra. 

//Float properties.
static float forwardSpeedMPS; //Forward velocity of the round in meters per second. (1 meter = 3.28 feet)
static float hopUpSpeedMPS; //Upward velocity of the round from the hop up/spin/Magnus effect in meters per second. 
static float camXPos;
static float camYPos;
static float camZPos;
static float camXRotDegrees;
static float camYRotDegrees;
static float camZRotDegrees;
*/





enum ConfigValueTypes{boolvalue = 0, intvalue = 1, floatvalue = 2};

bool Config::LoadSettingsFromDisk(string filename)
{
	ifstream file;
	file.open(filename);
	if (!file.is_open())
	{
		cout << "Couldn't open config file: " << filename << endl;
		return false;
	}

	string line;
	while (getline(file, line))
	{
		//Proper lines are formatted [type letter][name]=[value]. Ex: fForwardSpeedMPS=65.0.
		if (line.length() < 4) continue; //Bare minimum length. No error so that we can have white space. 

		//Parse the first character to see what file type this is.
		ConfigValueTypes valtype;
		char typechar = line[0];
		if (typechar == 'b') valtype = ConfigValueTypes::boolvalue;
		else if (typechar == 'i') valtype = ConfigValueTypes::intvalue;
		else if (typechar == 'f') valtype = ConfigValueTypes::floatvalue;
		else if (typechar == '#') continue; //Continue without error message so we can use # as a comment. 
		else
		{
			cout << "Couldn't parse val type from char: " << typechar << " from " << line <<
				". Should be b (bool), i (int) or f (float)." << endl;
			continue;
		}

		//Get the index of the equals sign, which is the space between the name and value. 
		size_t equalsindex = line.find_first_of('=');
		if (equalsindex == string::npos)
		{
			cout << "Couldn't find equals (=) symbol in line: " << line << endl;
			continue;
		}
		
		//Get the value name. 
		string valname = line.substr(1, equalsindex - 1); //Starts at 1 to exclude value character.
		
		//Get the value. 
		string val = line.substr(equalsindex + 1, line.length() - equalsindex);
		//cout << "Parsed value of " << valname << ": " << val << endl;

		//Remove all white space from the value.
		size_t whitespaceindex = val.find_first_of(' ');
		while (whitespaceindex != string::npos)
		{
			val.erase(whitespaceindex);
			whitespaceindex = val.find_first_of(' ');
		}

		switch (valtype)
		{
		case ConfigValueTypes::boolvalue:
			bool boolval;
			if (val == "0") boolval = false;
			else if (val == "1") boolval = true;
			else
			{
				cout << "Couldn't parse bool value from " << val << ". Should be 0 (false) or 1 (true)." << endl;
				continue;
			}

			boolSettings[valname] = boolval;
		case ConfigValueTypes::intvalue:
			int intval;
			try
			{
				intval = stoi(val);
			}
			catch(const invalid_argument e)
			{
				cout << "Couldn't parse value " << val << " to a integer." << endl;
				continue;
			}
			intSettings[valname] = intval;
			
		case ConfigValueTypes::floatvalue:
			float floatval;
			try
			{
				floatval = stof(val);
			}
			catch (const invalid_argument e)
			{
				cout << "Couldn't parse value " << val << " to a float." << endl;
				continue;
			}
			floatSettings[valname] = floatval;
		}
	}


	//Config::isLoaded = true;
	isLoaded = true;

	return true; 
}

bool Config::SaveSettingsToDisk(string filename)
{
	/*ofstream myfile;
	myfile.open(fileName);
	myfile << "Testing settings.\n";
	myfile.close();*/

	return true; //TODO: Actually make sure shit worked. 
}

bool Config::GetBoolSetting(string settingname)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	if (boolSettings.count(settingname) == 1)
	{
		return boolSettings[settingname];
	}
	else
	{
		//Save bool setting
		return false; //Default value. 
	}
}

void Config::SetBoolSetting(string settingname, bool value)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	boolSettings[settingname] = value;
	Config::SaveSettingsToDisk(fileName);
}

int Config::GetIntSetting(string settingname)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	if (intSettings.count(settingname) == 1)
	{
		return intSettings[settingname];
	}
	else
	{
		//Save int setting
		return 0; //Default value. 
	}
}

void Config::SetIntSetting(string settingname, int value)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	intSettings[settingname] = value;
	Config::SaveSettingsToDisk(fileName);
}

float Config::GetFloatSetting(string settingname)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	if (floatSettings.count(settingname) == 1)
	{
		return floatSettings[settingname];
	}
	else
	{
		//Save bool setting
		return 0.0; //Default value. 
	}
}

void Config::SetFloatSetting(string settingname, float value)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	floatSettings[settingname] = value;
	Config::SaveSettingsToDisk(fileName);
}