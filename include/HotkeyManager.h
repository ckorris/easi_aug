#include <HotkeyBindings.h>
#include <vector>
#include <memory>
#pragma once

using namespace std;

class HotkeyManager
{
public:
	void RegisterKeyBinding(char key, void(*onTrue)());
	void Process();
private:
	vector<unique_ptr<HotkeyBinding>> _bindings;
};