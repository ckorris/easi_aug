#if SPI_OUTPUT || GPIO_HOTKEYS 

#include <GPIOHelper.h>

#include <vector>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <sstream>
#include <sys/mman.h>

using namespace std;

//Forward declarations.
void* GetBase();
int GetFD()
void* _base;

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
			direction_file << "out";
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


volatile gpio_t* GPIOHelper::InitPin_Out(int memaddress, int bit)
{
	gpio_t volatile *pinLed = (gpio_t volatile *)((char *)GetBase() + (memaddress & (getpagesize() - 1)));

	pinLed->CNF = pinLed->CNF | bit; //Set to GPIO, not SPIO.
	pinLed->OE = pinLed->OE | bit; //Enable output.
	pinLed->OUT = pinLed->OUT & (~bit); //Set output to low to start.
	pinLed->INT_ENB = pinLed->INT_ENB & (~bit); //Disable interrupts.

	return pinLed;
}

volatile gpio_t* GPIOHelper::InitPin_In(int memaddress, int bit)
{
	gpio_t volatile *pinLed = (gpio_t volatile *)((char *)GetBase() + (memaddress & (getpagesize() - 1)));

	pinLed->CNF = pinLed->CNF | bit; //Set to GPIO, not SPIO.
	pinLed->OE = pinLed->OE & (~bit); //Disable output.
	pinLed->INT_ENB = pinLed->INT_ENB & (~bit); //Disable interrupts.
	//pinLed->IN = pinLed->IN | bit; //Set initial value to one, but should be readonly.
	//pinLed->OUT = pinLed->OUT & (~bit); //Make sure output is off.
	pinLed->INT_STA = pinLed->INT_STA | bit;
	pinLed->INT_LVL = pinLed->INT_LVL | bit;
	pinLed->INT_LVL = pinLed->INT_LVL | (bit << 8);
	pinLed->INT_LVL = pinLed->INT_LVL | (bit << 16);
	pinLed->INT_CLR = pinLed->INT_CLR | bit;

	return pinLed;
}

bool GPIOHelper::GetValue_Mem(volatile gpio_t *pinLed, int bit)
{
	uint32_t readval = pinLed->IN &= bit;
	std::cout << "Bit: " << bit << std::endl;
	std::cout << "CNF: " << pinLed->CNF << std::endl;
	std::cout << "OE: " << pinLed->OE << std::endl;
	std::cout << "In: " << pinLed->IN << std::endl;
	return readval == bit;
}


void* GetBase()
{
	if (_base == nullptr)
	{
		int pagesize = getpagesize();
		int pagemask = pagesize - 1;
		_base = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, GetFD(), (0x6000d004 & ~pagemask));
	}
	return _base;
}

int GetFD()
{
	if (_fd == default)
	{

		//Open direct access to memory. 
		int fd = open("/dev/mem", O_RDWR | O_SYNC);
		if (fd < 0) {
			//fprintf(stderr, "usage : $ sudo %s (with root privilege)\n", argv[0]);
			cout << "Can't open memory. Try running with root (sudo ./[appname])." << endl;
			exit(1);
		}
	}
	return _fd;
}

#endif
