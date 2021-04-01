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
static map<string, string> stringSettings;

enum ConfigValueTypes { boolvalue = 0, intvalue = 1, floatvalue = 2, stringvalue = 3 };

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
		else if (typechar == 's') valtype = ConfigValueTypes::stringvalue;
		else if (typechar == '#') continue; //Continue without error message so we can use # as a comment. 
		else
		{
			cout << "Couldn't parse val type from char: " << typechar << " from " << line <<
				". Should be b (bool), i (int), f (float) or s (string)." << endl;
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

		//cout << "Value " << val << " typechar: " << typechar <<  " type: " << valtype << endl;

		switch (valtype)
		{
		case ConfigValueTypes::boolvalue:
			bool boolval;
			if (val[0] == '0') boolval = false; //For whatever reason, directly comparing the string ie val == "0" breaks on Ubuntu. 
			else if (val[0] == '1') boolval = true;
			else
			{
				cout << "Couldn't parse bool value from " << val << ". Should be 0 (false) or 1 (true)." << endl;
				continue;
			}

			boolSettings[valname] = boolval;
			break;
		case ConfigValueTypes::intvalue:
			int intval;
			try
			{
				intval = stoi(val);
			}
			catch (const invalid_argument e)
			{
				cout << "Couldn't parse value " << val << " to a integer." << endl;
				continue;
			}
			intSettings[valname] = intval;
			break;
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
			break;

		case ConfigValueTypes::stringvalue:
			//For strings, it's nice and easy since there's no parsing. 
			stringSettings[valname] = val;
			break;
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

	ofstream settingsfile;
	settingsfile.open(fileName, ofstream::out | ofstream::trunc);

	//Write the bool values. 
	settingsfile << "#Booleans" << endl;
	map<string, bool>::iterator booliterator = boolSettings.begin();
	while (booliterator != boolSettings.end())
	{
		settingsfile << "b" << booliterator->first << "=" << booliterator->second << endl;
		booliterator++;
	}
	settingsfile << endl;

	settingsfile << "#Integers" << endl;
	map<string, int>::iterator intiterator = intSettings.begin();
	while (intiterator != intSettings.end())
	{
		settingsfile << "i" << intiterator->first << "=" << intiterator->second << endl;
		intiterator++;
	}
	settingsfile << endl;

	settingsfile << "#Floats" << endl;

	map<string, float>::iterator floatiterator = floatSettings.begin();
	while (floatiterator != floatSettings.end())
	{
		settingsfile << "f" << floatiterator->first << "=" << floatiterator->second << endl;
		floatiterator++;
	}
	settingsfile << endl;

	settingsfile << "#Strings" << endl;

	map<string, string>::iterator stringiterator = stringSettings.begin();
	while (stringiterator != stringSettings.end())
	{
		settingsfile << "s" << stringiterator->first << "=" << stringiterator->second << endl;
	}

	//settingsfile << "Testing settings.\n";
	settingsfile.close();

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
		//Save float setting
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

string Config::GetStringSetting(string settingname)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	if (stringSettings.count(settingname) == 1)
	{
		return stringSettings[settingname];
	}
	else
	{
		return ""; //Default value. 
	}
}

void Config::SetStringSetting(string settingname, string value)
{
	if (!isLoaded)
	{
		LoadSettingsFromDisk(fileName);
	}

	stringSettings[settingname] = value;
	Config::SaveSettingsToDisk(fileName);
}