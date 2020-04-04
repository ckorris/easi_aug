#pragma once

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>


using namespace sl;
using namespace std;

class CamUtilities
{
public:
	cv::Mat GetViewMatrix(); //Probably shouldn't use.

	//static cv::Mat GetProjectionMatrix(Camera cam);
	static cv::Mat GetProjectionMatrix(sl::CameraInformation info);
	static int2 CameraToScreenPos(sl::float3 relpos, cv::Mat projectionmat, int screenwidth, int screenheight);
};