#pragma once

#if SPI_OUTPUT
#include <GPIOHelper.h>
#endif

class HotkeyBinding
{
public:
	HotkeyBinding(void(*onTrue)());
	
	void Process();
protected:
	virtual bool Evaluate() = 0;
	bool lastState;
private:
	void(*_onTrue)();
};

class KeyBinding : public HotkeyBinding
{
public:
	KeyBinding(char key, void(*onTrue)());
	bool Evaluate();
	static char _lastKey;
	static void PreProcess();

private:
	char _key;
	
};


#if SPI_OUTPUT //TODO: Rename to include both the SPI and GPIO input.
class GPIOBinding : public HotkeyBinding
{
public:
	GPIOBinding(int memoryAddress, int bit, int bcmNumber, void(*onTrue)());
	bool Evaluate();
private:
	int _memoryAddress;
	int _bit;
	int _bcmNumber;
	bool _lastState;
	volatile gpio_t* _pinDef;
};


#endif
