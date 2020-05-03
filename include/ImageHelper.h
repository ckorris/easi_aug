#pragma once

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

using namespace sl;
using namespace std;

class ImageHelper
{
public:
	ImageHelper(sl::Camera *zedref);



	cv::Mat RotateImageToConfig(cv::Mat unrotatedmat);

	cv::Mat RotateUIToConfig(cv::Mat uimat);

	///Converts a screen space point relative to the ZED image to where it should be placed in the UI
	///to appear in the same place once the camera and UI rotations are finished. 
	cv::Point RawImagePointToRotated(cv::Point inpoint);


private:
	sl::Camera* zed;

	///Retrieves how much the camera image was scaled on each axis to make it fill the screen after being rotated. 
	float GetRotationScale();
};