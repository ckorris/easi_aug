#include <HotkeyBindings.h>
#include <opencv2/opencv.hpp>

#if SPI_OUTPUT
#include <GPIOHelper.h>
#endif

HotkeyBinding::HotkeyBinding(void(*onTrue)())
{
	_onTrue = onTrue;
	lastState = false;
}

void HotkeyBinding::Process()
{
	bool newState = Evaluate();

	if (newState != lastState)
	{
		if (newState == true)
		{
			_onTrue();
		}

		lastState = newState;
	}
}

char KeyBinding::_lastKey = ' ';

KeyBinding::KeyBinding(char key, void(*onTrue)())
	:HotkeyBinding(onTrue)
{
	_key = key;
	_lastKey = ' ';
}

void KeyBinding::PreProcess()
{
	KeyBinding::_lastKey = cv::waitKey(10);
}

bool KeyBinding::Evaluate()
{
	return KeyBinding::_lastKey == _key;
}

#if SPI_OUTPUT

GPIOBinding::GPIOBinding(int memoryAddress, int bit, int bcmNumber, void(*onTrue)())
	:HotkeyBinding(onTrue)
{
	_memoryAddress = memoryAddress;
	_bit = bit;
	_bcmNumber = bcmNumber;
	_lastValue = false;
	
	//Set up the pin.
	GPIOHelper::GPIOSetup_Mem(memoryAddress, GPIOHelper::GPIODirection::IN);
	_pinDef = GPIOHelper::InitPin_In(base, pagemask, memoryAddress, bit);
}

bool GPIOBinding::Evaluate()
{

}

#endif