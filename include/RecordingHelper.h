#pragma once

#include <sl/Camera.hpp>

using namespace sl;
using namespace std;

class RecordingHelper
{
public:
	RecordingHelper(Camera* zedref);

	bool IsRecordingSVO();

	ERROR_CODE StartRecording();

	void StopRecording();

	void ToggleRecording(bool state);

private:

	Camera* zed;
};