#include <RecordingHelper.h>

#include <Config.h>
#include <ctime>
#include <iostream>

using namespace sl;
using namespace std;

const string FILE_PREFIX = "EasiAug_";

RecordingHelper::RecordingHelper(Camera* zedref)
{
	zed = zedref;
}

bool RecordingHelper::IsRecordingSVO()
{
	return zed->getRecordingStatus().is_recording;
}

ERROR_CODE RecordingHelper::StartRecording()
{
	//Use the current date and time as part of the filename. 
	time_t now;
	struct tm* timeinfo;
	char buffer[100];
	time(&now);
	timeinfo = localtime(&now);

	strftime(buffer, sizeof(buffer), "%m_%d_%Y_%H_%M_%S", timeinfo);
	string timestring(buffer);

	//Also add the file prefix we want and the .svo file extension. 
	//For now we'll not take the path from the config file, because detecting if it's valid is different for Windows and Linux. TODO. 
	//string filepath = Config::svoRecordDirectory() + FILE_PREFIX + timestring + ".svo";
	string filepath = FILE_PREFIX + timestring + ".svo";
	//filepath = Config::svoRecordDirectory() + FILE_PREFIX + timestring + ".svo"
	char *filechar = &filepath[0];
	RecordingParameters recordparams(filechar, SVO_COMPRESSION_MODE::H264); //H264 because this is designed to run on Jetson Nano. 

	ERROR_CODE error = zed->enableRecording(recordparams);

	if (error == ERROR_CODE::SUCCESS)
	{
		cout << "Started recording to " + filepath << "." << endl;
	}
	else
	{
		cout << "Failed to start recording: " << error << endl;
		cout << "Path: " << filepath << endl;
	}

	return error;
}


void RecordingHelper::StopRecording()
{
	if (zed->getRecordingStatus().is_recording == true)
	{
		zed->disableRecording();
		cout << "Stopped recording." << endl;
	}
}