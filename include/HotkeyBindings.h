#pragma once

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