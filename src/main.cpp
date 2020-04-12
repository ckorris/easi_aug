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

	bool switchwidthandheight = Config::screenRotation() % 2 == 0 ? false : true;
	if (switchwidthandheight)
	{
		//image_ocv.cols = image_zed.getHeight();
		//image_ocv.rows = image_zed.getWidth();
	}

	Mat depth_measure(new_width, new_height, MAT_TYPE::F32_C1);

	//Make starting pose.
	sl::float3 startposition;
	startposition = sl::float3(0.06, -0.026, 0.0062);

	Simulation sim(&zed, startposition);

	// Loop until 'q' is pressed
	char key = ' ';
	while (key != 'q') {

		if (zed.grab(runtime_parameters) == ERROR_CODE::SUCCESS) {

			// Retrieve the left image, depth image in half-resolution
			zed.retrieveImage(image_zed, VIEW::LEFT, MEM::CPU, new_image_size);

			zed.retrieveMeasure(depth_measure);



			//Laser crosshair - no gravity. 
			if (Config::toggleLaserCrosshair() || Config::toggleLaserPath())
			{
				int2 collisionpoint_nograv;
				float collisiondepth_nograv;
				float traveltime_nograv; 
				bool collided = sim.Simulate(depth_measure, Config::forwardSpeedMPS(), 0.04, false, collisionpoint_nograv, collisiondepth_nograv, traveltime_nograv,
					Config::toggleLaserPath(), image_ocv, cv::Scalar(0, 255.0, 0));
				if(collided && Config::toggleLaserCrosshair())
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
				bool collided = sim.Simulate(depth_measure, Config::forwardSpeedMPS(), 0.04, true, collisionpoint_grav, collisiondepth_grav, traveltime_grav,
					Config::toggleGravityPath(), image_ocv, cv::Scalar(0, 0, 255.0));
				if (collided && Config::toggleGravityCrosshair())
				{
					float dotradius = 30 / collisiondepth_grav;

					//cout << "Collided at " << collisionpoint_grav.x << ", " << collisionpoint_grav.y << endl;
					cv::circle(image_ocv, cv::Point(collisionpoint_grav.x, collisionpoint_grav.y), dotradius, cv::Scalar(0, 0, 255.0), -1);
					if (Config::toggleDistance())
					{
						char distbuffer[20];
						int d = sprintf(distbuffer, "%0.2f", collisiondepth_grav);
						string distsuffix = "m";
						string disttext = distbuffer + distsuffix; //Not sure why I can't just add the literal. 
						cv::Size disttextsize = cv::getTextSize(disttext, 1, 1, 1, NULL);

						cv::Point disttextpoint(collisionpoint_grav.x - 20 - dotradius - disttextsize.width, 
							collisionpoint_grav.y - 20 - dotradius);
						cv::putText(image_ocv, disttext, disttextpoint, 1, 1, cv::Scalar(0, 0, 255));
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

						cv::putText(image_ocv, timetext, timetextpoint, 1, 1, cv::Scalar(0, 0, 255));
					}
				}
				else
				{
					//cout << "Did not collide. Last was " << collisionpoint_grav.x << ", " << collisionpoint_grav.y << endl;
				}
			}

			cv::namedWindow("EasiAug");

			//cv::namedWindow("EasiAug", cv::WINDOW_KEEPRATIO);
			//cv::setWindowProperty("EasiAug", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);

			//Test button. 
			//cv::Rect testrect;
			//testrect = cv::Rect(0, new_height - 50, 80, 50);
			//cv::rectangle(image_ocv, testrect, cv::Scalar(255, 0, 0), -1);

			//Draw drawables. 
			//cv::Rect panelrect = cv::Rect(1, new_height * 0.8, new_width - 2, new_height * 0.2 - 2); //Bottom. 
			cv::Rect panelrect = cv::Rect(2, 1, new_width - 2, new_height * 0.2 - 2);
			panel.ProcessUI(panelrect, image_ocv, "EasiAug");

			cv::imshow("EasiAug", image_ocv);

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
