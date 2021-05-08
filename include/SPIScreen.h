#if SPI_OUTPUT

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

//BCM GPIO for Jetson Nano
#define GPIO4 216 // 7, 4
#define GPIO17 50 // 11, 17
#define GPIO18 79 // 12, 18
#define GPIO27 14 // 13, 27
#define GPIO22 194 // 15, 22
#define GPIO23 232 // 16, 23
#define GPIO24 15 // 18, 24
#define SPI0_MOSI 16 // 19, 10
#define SPI0_MISO 17 // 21, 9
#define GPIO25 13 // 22, 25
#define SPI0_SCK 18 // 23, 11
#define SPI0_CS0 19 // 24, 8
#define SPI0_CS1 20 // 26, 7
#define GPIO5 149 // 29, 5
#define GPIO6 200 // 31, 6
#define GPIO12 168 // 32, 12
#define GPIO13 38 // 33, 13
#define GPIO19 76 // 35, 19
#define GPIO16 51 // 36, 16
#define GPIO26 12 // 37, 26
#define GPIO20 77 // 38, 20
#define GPIO21 78 // 40, 21

/*
//BCM numbering.
int DIN = SPI0_MOSI; //Pin 19
int CLK = SPI0_SCK; //Pin 23
int CS = SPI0_CS0; //Pin 24
int DC = GPIO25; //Pin 22
int RST = GPIO27; //Pin 13
int BL = GPIO24; //Pin 12
*/

#define DIN_MEM 0x6000d008 //PC.
#define CLK_MEM 0x6000d008 //PC.
#define CS_MEM 0x6000d008 //PC.
//#define DC_MEM 0x6000d004 //PB.
#define DC_MEM 0x6000d008 //PC.
#define RST_MEM 0x6000d004 //PB.
//#define BL_MEM 0x6000d204 //PJ.
#define BL_MEM 0x6000d004 //PB.

#define DIN_BIT 0x01 //Bit 0 - 00000001
#define CLK_BIT 0x04 //02. 
#define CS_BIT 0x08 //03.
#define DC_BIT 0x02 //01.
#define RST_BIT 0x80 //07.
#define BL_BIT 0x20 //05.

//For shared pins DIN, CLK CS, which all use 0x6000d008. 

//With DC off, for data.
#define DINOFF_CLKOFF_CSOFF_DCOFF 0x00
#define DINON_CLKOFF_CSOFF_DCOFF 0x01
#define DINOFF_CLKON_CSOFF_DCOFF 0x04 
#define DINON_CLKON_CSOFF_DCOFF 0x05
#define DINOFF_CLKOFF_CSON_DCOFF 0x08

//With DC on, for commands.
#define DINOFF_CLKOFF_CSOFF_DCON 0x02
#define DINON_CLKOFF_CSOFF_DCON 0x03
#define DINOFF_CLKON_CSOFF_DCON 0x06 
#define DINON_CLKON_CSOFF_DCON 0x07
#define DINOFF_CLKOFF_CSON_DCON 0x0A //Might need to be 0x10 but I don't think so.

//For shared pins DC and RST, which both use 0x6000d004.
//#define DCOFF_RSTON 0x40
//#define DCON_RSTON 0x60

#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <chrono>
#include <GPIOHelper.h>

/* //Moved to GPIOHelper.
typedef struct {
    uint32_t CNF;
    uint32_t _padding1[3];
    uint32_t OE;
    uint32_t _padding2[3];
    uint32_t OUT;
    uint32_t _padding3[3];
    uint32_t IN;
    uint32_t _padding4[3];
    uint32_t INT_STA;
    uint32_t _padding5[3];
    uint32_t INT_ENB;
    uint32_t _padding6[3];
    uint32_t INT_LVL;
    uint32_t _padding7[3];
    uint32_t INT_CLR;
    uint32_t _padding8[3];
} gpio_t;
*/

class SPIScreen
{
public: 
	SPIScreen();
	~SPIScreen();
	void LCD_Clear(uint16_t color);
	void LCD_ShowImage(std::vector<unsigned char> image, int rows, int cols, int channels);

private: 
	void InitGPIO();
	void CleanupGPIO();
	void InitLCD();
	void SendCommand(uint8_t byte);
	void SendData(uint8_t byte);
	void SendData_Word(uint16_t word);
	void SendData_Word_NoCmdCheck(uint16_t word);
	void SetWindow(uint16_t xstart, uint16_t  ystart, uint16_t xend, uint16_t yend);
	void Reset();

	void LogDuration(std::chrono::time_point<std::chrono::high_resolution_clock> start, 
		std::chrono::time_point<std::chrono::high_resolution_clock> finish);

	void SetCommandMode(bool isCommand);

	//volatile gpio_t* InitPin_Mem(void *base, int pagemask, int memaddress, int bit); //Moved to GPIOHelper.
	void SetValue_Mem(volatile gpio_t *pinLed, int bit, bool state);

	//void GPIOSetup_Mem(const int gpio);
	//void ExportGPIO_Mem(const int gpio);
	//void UnexportGPIO_Mem(const int gpio);
	
};


#endif