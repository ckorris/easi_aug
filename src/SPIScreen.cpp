//To compile: g++ -o SPIScreen SPIScreen.cpp -lJetsonGPIO

#include <SPIScreen.h>

#include <stdlib.h>   
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <sys/fcntl.h>
#include <sys/mman.h>

#include <chrono>

//During experiments with init. 
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

#include <thread>


//BOARD numbering.
int DIN = 19;
int CLK = 23;
int CS = 24;
int DC = 22;
int RST = 13;
int BL = 12;

//BCM numbering.
int DIN_BCM = 16;
int CLK_BCM = 18;
int CS_BCM = 19;
int DC_BCM = 13;
int RST_BCM = 14;
int BL_BCM = 79;



//Note some of these will end up being identical.
gpio_t volatile *dinPin;
gpio_t volatile *clkPin;
gpio_t volatile *csPin;
gpio_t volatile *dcPin;
gpio_t volatile *rstPin;
gpio_t volatile *blPin;

//TO DO: Make these stored in one place only. 
//Not sure why these are reversed, guess that's just the hardware. 
int LCD_WIDTH = 240;
int LCD_HEIGHT = 320;


using namespace std;

constexpr auto ROOT = "/sys/class/gpio"; //To do: Move this and remove auto. 

bool isSetToCommand = false; //Note: Be careful if the first packet sent is data. 

SPIScreen::SPIScreen()
{
	cout << "Initializing SPI screen." << endl;
	InitGPIO();

	//Open direct access to memory. 
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		//fprintf(stderr, "usage : $ sudo %s (with root privilege)\n", argv[0]);
		cout << "Can't open memory. Try running with root (sudo ./[appname])." << endl;
		exit(1);
	}

	//  map a particular physical address into our address space
	int pagesize = getpagesize();
	int pagemask = pagesize-1;

	//  This page will actually contain all the GPIO controllers, because they are co-located
	void *base = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (0x6000d004 & ~pagemask));
	if (base == NULL) {
	    perror("mmap()");
	    exit(1);
	}

	//Initialize pins.
	dinPin = InitPin_Mem(base, pagemask, DIN_MEM, DIN_BIT);
	clkPin = InitPin_Mem(base, pagemask, CLK_MEM, CLK_BIT);
	csPin = InitPin_Mem(base, pagemask, CS_MEM, CS_BIT);
	dcPin = InitPin_Mem(base, pagemask, DC_MEM, DC_BIT);
	rstPin = InitPin_Mem(base, pagemask, RST_MEM, RST_BIT);
	blPin = InitPin_Mem(base, pagemask, BL_MEM, BL_BIT);

	//Just to be sure, we make sure isSetToCommand matches the DC pin status. 
	//This is in case it's set to high somehow before our first command, as otherwise it 
	//would read isSetToCommand as false (default) and think it doesn't need to set the pin low. 
	isSetToCommand = (dcPin->OUT & DC_BIT) == DC_BIT;

	//Clear the screen's previous values and send commands to initialize the screen. 
	Reset();
	InitLCD(); 

	//Turn on back light. 
	SetValue_Mem(blPin, BL_BIT, true); 
	
}

SPIScreen::~SPIScreen()
{
	CleanupGPIO();

	std::cout << "Finished and cleaned up." << std::endl;
}

void SPIScreen::LCD_Clear(uint16_t color)
{
	//TEST

	//std::chrono::time_point<std::chrono::high_resolution_clock> start = 		//		std::chrono::high_resolution_clock::now();


	//Set the window for the whole thing. 	
	SetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);

	//Set the command status here, letting us skip the check when we send each word. 
	dcPin->OUT = DCON_RSTON; //Sets to high, indicating data. 

	for(int u = 0; u < LCD_WIDTH; u++)
	{
		//std::cout << "Column: " << u << std:: endl;
		for(int v = 0; v < LCD_HEIGHT; v++)
		{
			//This loses time because it's setting the DC to high each time. 
			//SendData_Word(color);
			SendData_Word_NoCmdCheck(color);
		}
	}
	dinPin->OUT = DINOFF_CLKOFF_CSON; //TEST

	std::chrono::time_point<std::chrono::high_resolution_clock> finish = 				std::chrono::high_resolution_clock::now();
	
	//LogDuration(start, finish);
	
}

void SPIScreen::LCD_ShowImage(vector<unsigned char> image, int rows, int cols, int channels)
{

	if(channels != 3)
	{
		cout << "Tried to display image with " << channels << " channels. ";
		cout << "Only 3 channel images supported right now." << endl;
		return;
	}

	//Set the window for the whole thing. 	
	SetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);
	//SetWindow(0, 0, LCD_HEIGHT, LCD_WIDTH);

	dcPin->OUT = DCON_RSTON; //Sets to high, indicating data. 


	bool hasprinted = false; //For test. 


for(int v = 0; v < rows; ++v)
	{
		for(int u = 0; u < cols; ++u)
		{	
			//int startindex = (v * rows * 3) + u * 3; //3 channels. 
			int startindex = (u * rows * 3) + v * 3; //3 channels. 

/*
	for(int u = 0; u < cols; ++u)
	{
		for(int v = 0; v < rows; ++v)
		{	
			int startindex = (u * cols * 3) + v * 3; //3 channels. 
*/
			//Convert three bits into RGB565. 
			int blue = image[startindex + 0]; //Need to cast?
			int green = image[startindex + 1];
			int red = image[startindex + 2];

			if(hasprinted == false)
			{
				cout << "B " << blue << "G " << green << "R " << red << endl;
			}

			//Shrink the color values so their min/max is same when converted to 565.
			blue = blue * (31.0 / 255); //31 is the largest number that 5 bytes can show. 
			green = green * (63.0 / 255); //63 is largest for 6 bytes.
			red = red * (31.0 / 255);

			uint16_t color = 0;
			color = color | blue;
			color = color | (green << 5);
			color = color | (red << 11);

			if(hasprinted == false)
			{
				cout << "B " << blue << "G " << green << "R " << red << endl;
				cout << "Color: " << color << endl;
				hasprinted = true;
			}

			SendData_Word_NoCmdCheck(color);
		}
	}

	dinPin->OUT = DINOFF_CLKOFF_CSON;
	
	
}

void SPIScreen::InitGPIO()
{
	GPIOSetup_Mem(DIN_BCM);
	GPIOSetup_Mem(CLK_BCM);
	GPIOSetup_Mem(CS_BCM);
	GPIOSetup_Mem(DC_BCM);
	GPIOSetup_Mem(RST_BCM);
	GPIOSetup_Mem(BL_BCM);
	
	
	printf("GPIO pins initialized.\r\n");
}

void SPIScreen::CleanupGPIO()
{
	UnexportGPIO_Mem(DIN_BCM);
	UnexportGPIO_Mem(CLK_BCM);
	UnexportGPIO_Mem(CS_BCM);
	UnexportGPIO_Mem(DC_BCM);
	UnexportGPIO_Mem(RST_BCM);
	UnexportGPIO_Mem(BL_BCM);
}




void SPIScreen::GPIOSetup_Mem(const int gpio)
{

	//Export the GPIO so we can start using it. 
	std::string path = std::string(ROOT) + "/gpio" + std::to_string(gpio);
	int pathexists = access(path.c_str(), F_OK);
	
	//if(pathexists == 0)
	if(pathexists != 0)
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
			if(time_count++ > 100) 
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
		direction_file << "out";
	} 
	

}

void SPIScreen::UnexportGPIO_Mem(const int gpio)
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



void SPIScreen::InitLCD()
{
	//Note this is for a very specific chip, ST7789VW, such as in WaveShare's 2inch TFT LCD.
	//If you want to use another screen, you'll likely need to refer to its data sheet
	//and send different commands here (along with checking for clock pulse and phase). 
	Reset();

	/*
	//MADCTRL
	SendCommand(0x36);
	SendData(0x00); 
	
	//CASET
	SendCommand(0x2A);
	SendData(0x00);
	SendData(0x00);
	SendData(0x01);
	SendData(0x3F);

	//RASET
	SendCommand(0x2B);
	SendData(0x00);
	SendData(0x00);
	SendData(0x00);
	SendData(0xEF);

	//PORCTRL
	SendCommand(0xB2);
	SendData(0x0C);
	SendData(0x0C);
	SendData(0x00);
	SendData(0x33);
	SendData(0x33);
	*/

	//COLMOD
	SendCommand(0x3A); 
	SendData(0x05); 

	//INVON
	SendCommand(0x21); 

	//SLPOUT
	SendCommand(0x11);

	//DISPON
	SendCommand(0x29);

}



void SPIScreen::LogDuration(std::chrono::time_point<std::chrono::high_resolution_clock> start, 
		std::chrono::time_point<std::chrono::high_resolution_clock> finish)
{
	//std::chrono::duration<double> elapsed = finish - start;
	std::chrono::duration<double, std::milli> elapsed = finish - start;

	std::cout << "Elapsed: " << (elapsed.count() / 1000.0) << " seconds. ";
	//std::cout << "Elapsed: " << elapsed_intms.count() << " ms" << std::endl;

	std::chrono::duration<long, std::micro> elapsed_usec = 				std::chrono::duration_cast<std::chrono::microseconds>(finish - start);

	std::chrono::duration<long, std::nano> elapsed_nsec = 				std::chrono::duration_cast<std::chrono::microseconds>(finish - start);

	//std::cout << "Elapsed (usec): " << elapsed_usec.count() << "us" << std::endl;

	//double microsecondsperpixel = elapsed_usec.count() / (LCD_WIDTH * LCD_HEIGHT);
	double microsecondsperpixel = elapsed_nsec.count() / 1000.0 / (LCD_WIDTH * LCD_HEIGHT);	
	//std::cout << "Elapsed time: " << milliseconds << " milliseconds" << std::endl;
	std::cout << "(" << microsecondsperpixel << " microseconds per pixel)" << std::endl;
}

void SPIScreen::SendCommand(uint8_t byte)
{
	//Make sure CS is low. 
	//SetValue_Mem(csPin, CS_BIT, false); //CS low indicates message of some kind. High is spacing.
	//SetValue_Mem(dcPin, DC_BIT, false);  //DC low indicates command, not data. 
	SetCommandMode(true);

	//for(int bit = 0; bit < 8; bit++) //LSB.
	for(int bit = 7; bit >= 0; bit--) //MSB.	
	{
		//bool value = (byte>>bit)&1;
		//if(value == true) 
		if((byte>>bit)&1 == true)
		{
			dinPin->OUT = DINON_CLKOFF_CSOFF;
			dinPin->OUT = DINON_CLKON_CSOFF;
		}
		else 
		{
			dinPin->OUT = DINOFF_CLKOFF_CSOFF;
			dinPin->OUT = DINOFF_CLKON_CSOFF;	
		}
		
	}

	//We don't set CS back to high for commands. 
}


void SPIScreen::SendData(uint8_t byte)
{
	SetCommandMode(false);

	//for(int bit = 0; bit < 8; bit++) //LSB.
	for(int bit = 7; bit >= 0; bit--) //MSB.
	{
		bool value = (byte>>bit)&1;
		
		//if(value == true) 
		if((byte>>bit)&1 == true)		
		{
			dinPin->OUT = DINON_CLKOFF_CSOFF;
			dinPin->OUT = DINON_CLKON_CSOFF;
		}
		else 
		{
			dinPin->OUT = DINOFF_CLKOFF_CSOFF;
			dinPin->OUT = DINOFF_CLKON_CSOFF;	
		}
	}

	//dinPin->OUT = DINOFF_CLKOFF_CSON;
}

void SPIScreen::SendData_Word(uint16_t word)
{	
	SetCommandMode(false);

	//for(int bit = 0; bit < 16; bit++) //LSB.
	for(int bit = 15; bit >= 0; bit--) //MSB.
	{
		//bool value = (word>>bit)&1; //This adds ~0.01 microseconds to the operation. Lol. 
		//if(value == true) 
		if((word>>bit)&1 == true)
		{
			dinPin->OUT = DINON_CLKOFF_CSOFF;
			dinPin->OUT = DINON_CLKON_CSOFF;
		}
		else 
		{
			dinPin->OUT = DINOFF_CLKOFF_CSOFF;
			dinPin->OUT = DINOFF_CLKON_CSOFF;	
		}
		
	}
		
	//dinPin->OUT = DINOFF_CLKOFF_CSON;
}

void SPIScreen::SendData_Word_NoCmdCheck(uint16_t word)
{
	//This version is exactly like the other but skips the command check, assuming we've
	//done it in encompassing logic somewhere, like before a for loop. 
	//SetCommandMode(false);

	//for(int bit = 0; bit < 16; bit++) //LSB.
	for(int bit = 15; bit >= 0; bit--) //MSB.
	{
		//bool value = (word>>bit)&1; //This adds ~0.01 microseconds to the operation. Lol. 
		//if(value == true) 
		if((word>>bit)&1 == true)
		{
			dinPin->OUT = DINON_CLKOFF_CSOFF;
			dinPin->OUT = DINON_CLKON_CSOFF;
		}
		else 
		{
			dinPin->OUT = DINOFF_CLKOFF_CSOFF;
			dinPin->OUT = DINOFF_CLKON_CSOFF;	
		}
		
	}
		
	//dinPin->OUT = DINOFF_CLKOFF_CSON;
}

void SPIScreen::SetWindow(uint16_t xstart, uint16_t  ystart, uint16_t xend, uint16_t yend)
{
	SendCommand(0x2a); //CASET
	SendData(xstart >> 8); 
	SendData(xstart & 0xff);
	SendData((xend - 1) >> 8);
	SendData((xend - 1) & 0xff);

	SendCommand(0x2b); //RASET
	SendData(ystart >> 8); 
	SendData(ystart & 0xff);
	SendData((yend - 1) >> 8);
	SendData((yend - 1) & 0xff);

	SendCommand(0x2C); //RAMWR - Write data. 
}


void SPIScreen::Reset()
{
	//GPIO::output(CS, GPIO::HIGH);
	SetValue_Mem(csPin, CS_BIT, true);

	usleep(100 * 1000); //100 milliseconds.
	//GPIO::output(RST, GPIO::LOW);
	SetValue_Mem(rstPin, RST_BIT, false);

	usleep(100 * 1000); //100 milliseconds.
	//GPIO::output(RST, GPIO::HIGH);
	SetValue_Mem(rstPin, RST_BIT, true);

	usleep(100 * 1000); //100 milliseconds.
}

void SPIScreen::SetCommandMode(bool isCommand)
{
	if(isSetToCommand != isCommand)
	{
		//TODO: Write whole byte in a way that preserves reset value but without the read. 
		if(isCommand == true)
		{
			//SetValue_Mem(dcPin, DC_BIT, false); //DC low indicates command, not data. 
			dcPin->OUT = DCOFF_RSTON;
		}
		else //isCommand == false
		{
			//SetValue_Mem(dcPin, DC_BIT, true); //DC high indicates data, not command. 
			dcPin->OUT = DCON_RSTON;
		}
		isSetToCommand = isCommand;
	}
}

volatile gpio_t* SPIScreen::InitPin_Mem(void *base, int pagemask, int memaddress, int bit)
{
	gpio_t volatile *pinLed = (gpio_t volatile *)((char *)base + (memaddress & pagemask));

	pinLed->CNF = pinLed->CNF | bit;
	pinLed->OE = pinLed->OE | bit; 
	pinLed->OUT = pinLed->OUT & (~bit);
	pinLed->INT_ENB = pinLed->INT_ENB & (~bit);

	return pinLed;
} 

void SPIScreen::SetValue_Mem(volatile gpio_t *pinLed, int bit, bool state)
{
	//std::cout << "Setting pin to " << state << ". Before: " << pinLed->OUT << std::endl;	

	if(state == true) //Turn it on.
	{
		pinLed->OUT = pinLed->OUT | bit;
		//pinLed->OUT = 0xFF;
	}
	else
	{
		pinLed->OUT = pinLed->OUT & (~bit); 
		//pinLed->OUT = 0x00;
	}
	
	//std::cout << "After: " << pinLed->OUT << std::endl;

}










