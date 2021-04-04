#include <HotkeyBindings.h>
#include <opencv2/opencv.hpp>

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
			std::cout << "Detected key down." << std::endl;
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



