
#include <CamUtilities.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <Config.h>

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

cv::Mat CamUtilities::GetProjectionMatrix(sl::CameraInformation info)
{
	//sl::CameraInformation info = cam.getCameraInformation();
	sl::CalibrationParameters parameters = info.camera_configuration.calibration_parameters;

	cv::Mat projection = cv::Mat(4, 4, CV_32FC1);

	float fovx = parameters.left_cam.h_fov * (3.1415926 / 180.0);
	float fovy = parameters.left_cam.v_fov * (3.1415926 / 180.0);

	float f_imageWidth = info.camera_configuration.resolution.width;
	float f_imageHeight = info.camera_configuration.resolution.height;

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

sl::int2 CamUtilities::CameraToScreenPos(sl::float3 relpos, cv::Mat projectionmat, int screenwidth, int screenheight)
{
	cv::Mat relposmat = cv::Mat(4, 1, CV_32FC1);
	relposmat.at<float>(0, 0) = relpos[0];
	relposmat.at<float>(1, 0) = relpos[1];
	relposmat.at<float>(2, 0) = -relpos[2]; //Negative so that -1 is behind the cam and 1 is in front of it. 
	relposmat.at<float>(3, 0) = 1;

	cv::Mat multmat = projectionmat * relposmat;
	
	float w = multmat.at<float>(3, 0);

	//Check if point is outside the view frustum.
	if (w <= 0.0f) 
	{
		//Point is behind the camera, clip it to the near plane.
		//This lets us draw to a point that's off screen.
		w = nearPlane;
	}


	float vp_x = multmat.at<float>(0, 0) / w;
	float vp_y = -multmat.at<float>(1, 0) / w;

	int screen_x = (vp_x + 1) / 2.0 * screenwidth;
	int screen_y = (vp_y + 1) / 2.0 * screenheight;

	sl::int2 screenpos; 
	screenpos.x = screen_x;
	screenpos.y = screen_y;

	return screenpos;
}

//Provides a normal pointing in the direction of gravity, given the pose from the ZED2's IMU. 
[[deprecated("Did weird things when you turned around because it's trying to track pose over time. Use imu.linear_acceleration instead.")]]
sl::float3 CamUtilities::IMUPoseToGravityVector(sl::Pose imupose)
{
	//sl::float3 eulerrotrad = imupose.getEulerAngles(true);
	//sl::float3 downvec(0, -1, 0);
	sl::Translation downvec(0, -1, 0); //Using a translation because it's multipliable. 

	sl::Translation gravvec;
	gravvec = downvec * imupose.getOrientation();

	sl::float3 returnnormal(gravvec.x, gravvec.y, gravvec.z);
	return returnnormal;
}

int CamUtilities::InitZed(int argc, char **argv, Camera *zed, RuntimeParameters *runtime_parameters, Resolution *image_size)
{
	// Set configuration parameters
	InitParameters initparams;
	initparams.camera_resolution = Config::camResolution();
	initparams.depth_mode = Config::camPerformanceMode();
	initparams.coordinate_units = UNIT::METER;
	if (argc > 1) initparams.input.setFromSVOFile(argv[1]);

	// Open the camera
	ERROR_CODE zed_open_state = zed->open(initparams);
	if (zed_open_state != ERROR_CODE::SUCCESS) {
		cout << "Can't open ZED camera.: " << zed_open_state << "\nExiting program." << endl;
		return -1;
	}

	//Increase saturation and sharpness a bit
	//TODO: Make these all config settings. 
	zed->setCameraSettings(VIDEO_SETTINGS::SATURATION, 7);
	zed->setCameraSettings(VIDEO_SETTINGS::SHARPNESS, 6);

	// Set runtime parameters after opening the camera

	runtime_parameters->enable_fill_mode = true;

	CameraInformation cameraInfo = zed->getCameraInformation();
	Resolution size = cameraInfo.camera_configuration.resolution;
	*image_size = size;


	return 1;
}

int CamUtilities::InitZed(Camera *zed, RuntimeParameters *runtime_parameters, Resolution *image_size)
{
	int argc = 0;
	char **argv; //Empty but whatever.
	return InitZed(argc, argv, zed, runtime_parameters, image_size);
}

int CamUtilities::slMatType2cvMatType(sl::MAT_TYPE sltype)
{
	int cv_type = -1;
	switch (sltype) {
	case sl::MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
	case sl::MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
	case sl::MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
	case sl::MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
	case sl::MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
	case sl::MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
	case sl::MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
	case sl::MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
	default: break;
	}
	return cv_type;
}