#if SPI_OUTPUT || GPIO_HOTKEYS 

#include <GPIOHelper.h>

#include <vector>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <sstream>

using namespace std;

constexpr auto ROOT = "/sys/class/gpio"; //To do: Move this and remove auto. 

void GPIOHelper::GPIOSetup_Mem(const int gpio, GPIODirection direction)
{

	//Export the GPIO so we can start using it. 
	std::string path = std::string(ROOT) + "/gpio" + std::to_string(gpio);
	int pathexists = access(path.c_str(), F_OK);

	//if(pathexists == 0)
	if (pathexists != 0)
	{
		//return;
		//Scope of f_export.
		{
			std::ofstream f_export(std::string(ROOT) + "/export");
			f_export << gpio;
		}

		std::string value_path = std::string(ROOT) + "/gpio" + std::to_string(gpio) + "/value";

		int time_count = 0;

		//while (access(value_path.c_str(), R_OK | W_OK) == 0)
		while (access(value_path.c_str(), R_OK | W_OK) != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (time_count++ > 100)
			{
				throw std::runtime_error("Permission denied to " + value_path + "\n Change permissions or run as root.");
			}
		}
	}

	//Set the direction of the pin to OUT. 

	std::string gpio_dir_path = std::string(ROOT) + "/gpio" + std::to_string(gpio) + "/direction";

	//Scope for direction_file.
	{
		//std::cout << "Set direction of pin " << gpio << " to out." << std::endl;
		std::ofstream direction_file(gpio_dir_path);
		//direction_file << "out";
		switch (direction)
		{
		case GPIODirection::IN:
			direction_file << "in";
			break;
		case GPIODirection::OUT:
			direction_file >> "out";
			break;
		}
	}


}

void GPIOHelper::UnexportGPIO_Mem(const int gpio)
{
	std::string path = std::string(ROOT) + "/gpio" + std::to_string(gpio);
	int pathexists = access(path.c_str(), F_OK);
	if (pathexists == 0)
	{
		return;
	}

	std::ofstream f_unexport(std::string(ROOT) + "/unexport");
	f_unexport << gpio;
}

#endif
