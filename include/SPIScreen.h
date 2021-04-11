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
#define DC_MEM 0x6000d004 //PB.
#define RST_MEM 0x6000d004 //PB.
#define BL_MEM 0x6000d204 //PJ.

#define DIN_BIT 0x01 //Bit 0 - 00000001
#define CLK_BIT 0x04 //02. 
#define CS_BIT 0x08 //03.
#define DC_BIT 0x20 //05.
#define RST_BIT 0x40 //06.
#define BL_BIT 0x80 //07.

//For shared pins DIN, CLK and CS, which all use 0x6000d008. 
#define DINOFF_CLKOFF_CSOFF 0x00
#define DINON_CLKOFF_CSOFF 0x01

#define DINOFF_CLKON_CSOFF 0x04 
#define DINON_CLKON_CSOFF 0x05

#define DINOFF_CLKOFF_CSON 0x08

//For shared pins DC and RST, which both use 0x6000d004.
#define DCOFF_RSTON 0x40
#define DCON_RSTON 0x60

#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <chrono>
#include <GPIOHelper.h>

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

	volatile gpio_t* InitPin_Mem(void *base, int pagemask, int memaddress, int bit);
	void SetValue_Mem(volatile gpio_t *pinLed, int bit, bool state);

	//void GPIOSetup_Mem(const int gpio);
	//void ExportGPIO_Mem(const int gpio);
	//void UnexportGPIO_Mem(const int gpio);
	
};


#endif