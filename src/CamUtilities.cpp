
#include <CamUtilities.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

using namespace sl;
using namespace std;

const float nearPlane = 0.1;
const float farPlane = 500;

cv::Mat CamUtilities::GetViewMatrix()
{
	cout << "VIEW" << endl;

	//The barrel is always fixed relative to the ZED, so we don't want to actually move it. 
	//So we'll make an empty one. 
	cv::Mat viewmat = cv::Mat(4, 4, CV_32FC1);
	viewmat.data[0, 0] = 1;
	viewmat.data[1, 1] = 1;
	viewmat.data[2, 2] = 1;
	viewmat.data[3, 3] = 1;

	return viewmat;
}

//cv::Mat CamUtilities::GetProjectionMatrix(Camera cam)
cv::Mat CamUtilities::GetProjectionMatrix(sl::CameraInformation info)
{
	//sl::CameraInformation info = cam.getCameraInformation();
	sl::CalibrationParameters parameters = info.camera_configuration.calibration_parameters;

	cv::Mat projection = cv::Mat(4, 4, CV_32FC1);

	float fovx = parameters.left_cam.h_fov * (3.1415926 / 180.0);
	float fovy = parameters.left_cam.v_fov * (3.1415926 / 180.0);

	float f_imageWidth = info.camera_resolution.width;
	float f_imageHeight = info.camera_resolution.height;

	//Manually construct the matrix based on initialization/calibration values.
	projection.at<float>(0, 0) = 1.0f / tan(fovx * 0.5f); //Horizontal FoV.
	projection.at<float>(0, 1) = 0;
	projection.at<float>(0, 2) = 2.0f * ((f_imageWidth - 1.0f * parameters.left_cam.cx) / f_imageWidth) - 1.0f; //Horizontal offset.
	//projection.at<float>(0, 2) = 0; //Horizontal offset.
	projection.at<float>(0, 3) = 0;

	projection.at<float>(1, 0) = 0;
	projection.at<float>(1, 1) = 1.0f / tan(fovy * 0.5f); //Vertical FoV.
	projection.at<float>(1, 2) = -(2.0f * ((f_imageHeight - 1.0f * parameters.left_cam.cy) / f_imageHeight) - 1.0f); //Vertical offset.
	//projection.at<float>(1, 2) = 0; //Vertical offset.
	projection.at<float>(1, 3) = 0;

	projection.at<float>(2, 0) = 0;
	projection.at<float>(2, 1) = 0;
	projection.at<float>(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane); //Near and far planes.
	projection.at<float>(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane); //Near and far planes.

	projection.at<float>(3, 0) = 0;
	projection.at<float>(3, 1) = 0;
	projection.at<float>(3, 2) = -1;
	projection.at<float>(3, 3) = 0.0f;

	 

	return projection;
}

int2 CamUtilities::CameraToScreenPos(sl::float3 relpos, cv::Mat projectionmat, int screenwidth, int screenheight)
{
	cv::Mat relposmat = cv::Mat(4, 1, CV_32FC1);
	relposmat.at<float>(0, 0) = relpos[0];
	relposmat.at<float>(1, 0) = relpos[1];
	relposmat.at<float>(2, 0) = -relpos[2]; //Negative so that -1 is behind the cam and 1 is in front of it. 
	relposmat.at<float>(3, 0) = 1;

	cv::Mat multmat = projectionmat * relposmat;
	
	float w = multmat.at<float>(3, 0);

	float vp_x = multmat.at<float>(0, 0) / w;
	float vp_y = -multmat.at<float>(1, 0) / w;

	int screen_x = (vp_x + 1) / 2.0 * screenwidth;
	int screen_y = (vp_y + 1) / 2.0 * screenheight;

	int2 screenpos; 
	screenpos.x = screen_x;
	screenpos.y = screen_y;

	return screenpos;
}

