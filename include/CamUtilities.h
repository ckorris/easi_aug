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
	static sl::int2 CameraToScreenPos(sl::float3 relpos, cv::Mat projectionmat, int screenwidth, int screenheight);

	static sl::float3 IMUPoseToGravityVector(sl::Pose imupose);

	static int InitZed(int argc, char **argv, Camera *zed, RuntimeParameters *runtime_parameters, Resolution *image_size);

	static int InitZed(Camera *zed, RuntimeParameters *runtime_parameters, Resolution *image_size);

	static int slMatType2cvMatType(sl::MAT_TYPE sltype);
};