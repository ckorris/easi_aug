///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/***********************************************************************************************
 ** This sample demonstrates how to use the ZED SDK with OpenCV. 					  	      **
 ** Depth and images are captured with the ZED SDK, converted to OpenCV format and displayed. **
 ***********************************************************************************************/

 // ZED includes
#include <sl/Camera.hpp>

// OpenCV includes
#include <opencv2/opencv.hpp>

#include <CamUtilities.h>
#include <Simulation.h>
#include <Config.h>
#include <Drawables.h>

using namespace std;
using namespace sl;

cv::Mat slMat2cvMat(Mat& input);
int slMatType2cvMatType(sl::MAT_TYPE sltype);
void printHelp();

void ClickCallback(int event, int x, int y, int flags, void* userdata); //Forward declaration. 

EmptyPanel panel(0, 1, 0, 1);

int main(int argc, char **argv) {

	// Create a ZED camera object
	Camera zed;

	// Set configuration parameters
	InitParameters init_parameters;
	init_parameters.camera_resolution = Config::camResolution();
	init_parameters.depth_mode = Config::camPerformanceMode();
	init_parameters.coordinate_units = UNIT::METER;
	if (argc > 1) init_parameters.input.setFromSVOFile(argv[1]);

	// Open the camera
	ERROR_CODE zed_open_state = zed.open(init_parameters);
	if (zed_open_state != ERROR_CODE::SUCCESS) {
		cout << "[Sample] Error Camera Open: " << zed_open_state << "\nExit program." << endl;
		return -1;
	}

	// Display help in console
	//printHelp();

	// Set runtime parameters after opening the camera
	RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = SENSING_MODE::FILL;


	// Prepare new image size to retrieve half-resolution images
	Resolution image_size = zed.getCameraInformation().camera_configuration.resolution;



	int new_width = image_size.width;
	int new_height = image_size.height;

	Resolution new_image_size(new_width, new_height);

	// To share data between sl::Mat and cv::Mat, use slMat2cvMat()
	// Only the headers and pointer to the sl::Mat are copied, not the data itself
	Mat image_zed(new_image_size, MAT_TYPE::U8_C4);
	//cv::Mat image_ocv = slMat2cvMat(image_zed);

	int cvmattype = slMatType2cvMatType(image_zed.getDataType());
	cv::Mat image_ocv = cv::Mat(new_height, new_width, cvmattype, image_zed.getPtr<sl::uchar1>(MEM::CPU));

	//Make the UI image. 
	int screenrot = Config::screenRotation();
	int uiwidth = (screenrot % 2 == 0) ? image_ocv.cols : image_ocv.rows;
	int uiheight = (screenrot % 2 == 0) ? image_ocv.rows : image_ocv.cols;

	cv::Mat ui_mat = cv::Mat(uiheight, uiwidth, CV_8UC4);
	ui_mat.setTo(cv::Scalar(0, 0, 0, 0));

	Mat depth_measure(new_width, new_height, MAT_TYPE::F32_C1);

	//Make starting pose.
	/*sl::float3 startposition;
	startposition = sl::float3(0.06, -0.026, 0.0062);
	Simulation sim(&zed, startposition);*/

	Simulation sim(&zed);

	SensorsData sensorData;

	// Loop until 'q' is pressed
	char key = ' ';
	while (key != 'q') {

		if (zed.grab(runtime_parameters) == ERROR_CODE::SUCCESS) {

			//Test sensors. 
			//zed.getSensorsData(sensorData, TIME_REFERENCE::IMAGE);
			//cout << sensorData.imu.linear_acceleration << endl;
			//cout << sensorData.barometer.pressure << endl;
			//float temperature;
			//sensorData.temperature.get(SensorsData::TemperatureData::SENSOR_LOCATION::BAROMETER, temperature);
			//cout << "Temp: " << temperature << " C" << endl;
			//cout << sensorData.magnetometer.magnetic_field_calibrated << endl;
			Pose pose = sensorData.imu.pose;
			//cout << pose.getEulerAngles(false) << endl;
			//sl::float4 gravdir = pose.getRotationMatrix() * sl::float4(0, -1, 0, 1);
			//sl::float3 gravnorm = CamUtilities::IMUPoseToGravityVector(pose);
			//cout << gravnorm << endl;


			// Retrieve the left image, depth image in half-resolution
			zed.retrieveImage(image_zed, VIEW::LEFT, MEM::CPU, new_image_size);

			zed.retrieveMeasure(depth_measure);

			zed.getSensorsData(sensorData, TIME_REFERENCE::IMAGE);

			ui_mat.setTo(cv::Scalar(0, 0, 0, 0));

			//Laser crosshair - no gravity. 
			if (Config::toggleLaserCrosshair() || Config::toggleLaserPath())
			{
				int2 collisionpoint_nograv;
				float collisiondepth_nograv;
				float traveltime_nograv;
				bool collided = sim.Simulate(depth_measure, Config::forwardSpeedMPS(), 0.04, false, sensorData, collisionpoint_nograv, collisiondepth_nograv, traveltime_nograv,
					Config::toggleLaserPath(), image_ocv, cv::Scalar(0, 255.0, 0, 1));
				if (collided && Config::toggleLaserCrosshair())
				{
					//cout << "Collided at " << collisionpoint_nograv.x << ", " << collisionpoint_nograv.y << endl;
					cv::circle(image_ocv, cv::Point(collisionpoint_nograv.x, collisionpoint_nograv.y), 30 / collisiondepth_nograv, cv::Scalar(0, 255.0, 0), -1);
				}
				else
				{
					//cout << "Did not collide. Last was " << collisionpoint_nograv.x << ", " << collisionpoint_nograv.y << endl;
				}
			}

			//Laser crosshair - gravity.
			if (Config::toggleGravityCrosshair() || Config::toggleGravityPath())
			{
				int2 collisionpoint_grav;
				float collisiondepth_grav;
				float traveltime_grav;
				bool collided = sim.Simulate(depth_measure, Config::forwardSpeedMPS(), 0.04, true, sensorData, collisionpoint_grav, collisiondepth_grav, traveltime_grav,
					Config::toggleGravityPath(), image_ocv, cv::Scalar(0, 0, 255.0, 1));
				if (collided && Config::toggleGravityCrosshair())
				{
					float dotradius = 30 / collisiondepth_grav;

					//cout << "Collided at " << collisionpoint_grav.x << ", " << collisionpoint_grav.y << endl;
					cv::circle(image_ocv, cv::Point(collisionpoint_grav.x, collisionpoint_grav.y), dotradius, cv::Scalar(0, 0, 255.0, 1), -1);
					if (Config::toggleDistance())
					{
						char distbuffer[20];
						int d = sprintf(distbuffer, "%0.2f", collisiondepth_grav);
						string distsuffix = "m";
						string disttext = distbuffer + distsuffix; //Not sure why I can't just add the literal. 
						cv::Size disttextsize = cv::getTextSize(disttext, 1, 1, 1, NULL);

						cv::Point disttextpoint(collisionpoint_grav.x - 20 - dotradius - disttextsize.width,
							collisionpoint_grav.y - 20 - dotradius);

						//Rotate coords if we're rotating the screen. 
						if (screenrot % 2 != 0)
						{
							float oldx = disttextpoint.x;
							disttextpoint.x = disttextpoint.y;
							disttextpoint.y = oldx;
						}

						cv::putText(ui_mat, disttext, disttextpoint, 1, 1, cv::Scalar(0, 0, 255, 1));
					}
					if (Config::toggleTravelTime())
					{
						char timebuffer[20];
						int t = sprintf(timebuffer, "%0.3f", traveltime_grav);
						string timesuffix = "ms";
						string timetext = timebuffer + timesuffix;
						//cv::Size timetextsize = cv::getTextSize(timetext, 1, 1, 1, NULL);

						cv::Point timetextpoint(collisionpoint_grav.x + 20 + dotradius,
							collisionpoint_grav.y - 20 - dotradius);

						//Rotate coords if we're rotating the screen. 
						if (screenrot % 2 != 0)
						{
							float oldx = timetextpoint.x;
							timetextpoint.x = timetextpoint.y;
							timetextpoint.y = oldx;
						}

						cv::putText(ui_mat, timetext, timetextpoint, 1, 1, cv::Scalar(0, 0, 255, 1));
					}
				}
				else
				{
					//cout << "Did not collide. Last was " << collisionpoint_grav.x << ", " << collisionpoint_grav.y << endl;
				}
			}

			//Below should be uncommented for non-full screen window. 
			cv::namedWindow("EasiAug");

			//Below should be uncommented if you want to have it be fullscreen automatically. Better for Jetson but worse for development on desktop.
			//cv::namedWindow("EasiAug", cv::WINDOW_KEEPRATIO);
			//cv::setWindowProperty("EasiAug", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

			cv::Rect panelrect;
			panelrect = cv::Rect(2, 1, ui_mat.cols - 2, ui_mat.rows * 0.2 - 2);
			/*if (scale == 1)
			{
				panelrect = cv::Rect(2, 1, ui_mat.cols - 2, ui_mat.rows * 0.2 - 2);
			}
			else
			{
				
				int scaledwidth = image_ocv.rows / scale;
				int leftoffset = image_ocv.cols / 2.0 - scaledwidth / 2.0;
				
				//int leftoffset = image_ocv.cols / 2 - image_ocv.rows / 2;

				panelrect = cv::Rect(leftoffset + 1, 1, scaledwidth - 2, image_ocv.rows * 0.15 - 2);
			}*/
			//panel.ProcessUI(panelrect, image_ocv, "EasiAug");
			panel.ProcessUI(panelrect, ui_mat, "EasiAug");

			//Try rotating the image. 
			cv::Mat finalmat;
			int imagerotamount = Config::imageRotation();
			if (imagerotamount > 0)
			{
				float rotangle = imagerotamount * 90;
				float imagewidthscale = (imagerotamount % 2 == 0) ? 1.0 : image_ocv.cols / (float)image_ocv.rows;

				//cv::Mat rotated;
				cv::Mat rotated = (imagerotamount == 2) ? cv::Mat(cv::Size(image_ocv.cols, image_ocv.rows), CV_8UC4) : cv::Mat(cv::Size(image_ocv.rows, image_ocv.cols), CV_8UC4);
				cv::Point2d pc; 
				if (imagerotamount == 1)
				{
					//pc = cv::Point2d(image_ocv.rows / 2, image_ocv.rows / 2); //If also changing res. 
					pc = cv::Point2d(image_ocv.cols / 2, image_ocv.rows / 2);
				}
				else if (imagerotamount == 2)
				{
					pc = cv::Point2d(image_ocv.cols / 2, image_ocv.rows / 2);
				}
				else if (imagerotamount == 3)
				{
					//pc = cv::Point2d(image_ocv.rows, image_ocv.rows); //If also changing res. 
					pc = cv::Point2d(image_ocv.cols / 2, image_ocv.rows / 2);
				}
				//cv::Point2d pc(image_ocv.rows / 2.0, image_ocv.cols / 2.0);
				cv::Mat screenrotmatrix = cv::getRotationMatrix2D(pc, -rotangle, imagewidthscale);

				cv::warpAffine(image_ocv, rotated, screenrotmatrix, image_ocv.size());

				//cv::add(rotated, ui_mat, finalmat);

				finalmat = rotated;
			}
			else
			{
				//cv::add(ui_mat, image_ocv, finalmat);
				//cv::addWeighted(image_ocv, 1.0, ui_mat, 1, 0.0, finalmat);
				finalmat = image_ocv;

				//finalmat = image_ocv;
			}

			
			cv::Mat mask(cv::Size(ui_mat.cols, ui_mat.rows), CV_8UC1);
			int fromto[] = { 3, 0 };
			cv::mixChannels(ui_mat, mask, fromto, 1);
			cv::subtract(finalmat, cv::Scalar(255, 255, 255, 255), finalmat, mask);
			cv::add(finalmat, ui_mat, finalmat, mask);
			

			//finalmat = finalmat + ui_mat;


			//cv::imshow("EasiAug", image_ocv);
			//cv::imshow("EasiAug", rotated);
			cv::imshow("EasiAug", finalmat);
			//cv::imshow("EasiAug", finalmat);

			//Input.
			cv::setMouseCallback("EasiAug", ClickCallback);


			// Handle key event
			key = cv::waitKey(10);
		}
	}
	zed.close();
	return 0;
}

void ClickCallback(int event, int x, int y, int flags, void* userdata)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		//Clickable::ProcessAllClicks(x, y);
		panel.ProcessAllClicks(x, y);
	}
}

/**
* Conversion function between sl::Mat and cv::Mat
**/
cv::Mat slMat2cvMat(Mat& input) {
	// Mapping between MAT_TYPE and CV_TYPE
	int cv_type = -1;
	switch (input.getDataType()) {
	case MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
	case MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
	case MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
	case MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
	case MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
	case MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
	case MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
	case MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
	default: break;
	}

	// Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
	// cv::Mat and sl::Mat will share a single memory structure
	return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM::CPU));
}

int slMatType2cvMatType(sl::MAT_TYPE sltype)
{
	int cv_type = -1;
	switch (sltype) {
	case MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
	case MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
	case MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
	case MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
	case MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
	case MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
	case MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
	case MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
	default: break;
	}
	return cv_type;
}

/**
* This function displays help in console
**/
void printHelp() {
	cout << " Press 's' to save Side by side images" << endl;
	cout << " Press 'p' to save Point Cloud" << endl;
	cout << " Press 'd' to save Depth image" << endl;
	cout << " Press 'm' to switch Point Cloud format" << endl;
	cout << " Press 'n' to switch Depth format" << endl;
}
