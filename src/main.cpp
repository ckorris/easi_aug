#include <sl/Camera.hpp>

#include <opencv2/opencv.hpp>

#include <CamUtilities.h>
#include <Simulation.h>
#include <Config.h>
#include <Drawables.h>
#include <ImageHelper.h>
#include <TimeHelper.h>
#include <RecordingHelper.h>
//#include <Menus.h>

using namespace std;
using namespace sl;

cv::Mat slMat2cvMat(Mat& input);
int slMatType2cvMatType(sl::MAT_TYPE sltype);

//ImageHelper imageHelper;

void ClickCallback(int event, int x, int y, int flags, void* userdata); //Forward declaration. 

//cv::Rect(2, 1, ui_mat.cols - 2, ui_mat.rows * 0.2 - 2);
cv::Rect tempmenurect(50, 0, 450, 180);
Sidebar panel(tempmenurect, 0, 1, 0, 1);

RecordingHelper* recordHelper; //Not actually used. 

bool(*recordinggetter)() = []() { return recordHelper->IsRecordingSVO(); };
void(*recordingsetter)(bool) = [](bool v) { recordHelper->ToggleRecording(v); };
ToggleButton_Record recordButton(recordinggetter, recordingsetter, 0, 1, 0, 1);

int main(int argc, char **argv) {

	// Create a ZED camera object
	Camera zed;

	// Set configuration parameters
	InitParameters initparams;
	initparams.camera_resolution = Config::camResolution();
	initparams.depth_mode = Config::camPerformanceMode();
	initparams.coordinate_units = UNIT::METER;
	if (argc > 1) initparams.input.setFromSVOFile(argv[1]);

	// Open the camera
	ERROR_CODE zed_open_state = zed.open(initparams);
	if (zed_open_state != ERROR_CODE::SUCCESS) {
		cout << "Can't open ZED camera.: " << zed_open_state << "\nExiting program." << endl;
		return -1;
	}


	// Set runtime parameters after opening the camera
	RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = SENSING_MODE::FILL;


	// Prepare new image size to retrieve half-resolution images
	Resolution image_size = zed.getCameraInformation().camera_configuration.resolution;

	int zedwidth = image_size.width;
	int zedheight = image_size.height;

	//Resolution new_image_size(zedwidth, zedheight);
	Mat zedimage(image_size, MAT_TYPE::U8_C4);

	int cvmattype = slMatType2cvMatType(zedimage.getDataType());
	cv::Mat image_ocv = cv::Mat(zedheight, zedwidth, cvmattype, zedimage.getPtr<sl::uchar1>(MEM::CPU));

	cv::Mat ui_mat; //Declared so we keep using the same memory each loop. 

	Mat depth_measure(zedwidth, zedheight, MAT_TYPE::F32_C1);

	Simulation sim(&zed);

	ImageHelper imageHelper(&zed);

	SensorsData sensorData;

	//Declare the recording helper (for recording SVO files) and button.
	RecordingHelper recorder(&zed);
	RecordingHelper record = static_cast<RecordingHelper>(recorder);
	recordHelper = &recorder;

	//TODO: I HATE having to pass an explicit reference to RecordingHelper to the Ui. 
	//That means the UI has to know what a RecordingHelper is. But passing a reference to it is proving very hard. 
	//Find a way around that that's not messy. 
	//bool(*recordinggetter)();
	bool(*recordinggetter)() = []() { return recordHelper->IsRecordingSVO(); };
	void(*recordingsetter)(bool) = [](bool v) { recordHelper->ToggleRecording(v); };
	//void(*setrecording)(bool) = [](bool v) {if (v) cout << "True" << endl; else cout << "False" << endl; };
	//bool(*isrecording)() = static_cast<RecordingHelper*>(&recorder)->IsRecordingSVO(); ;
	//isrecording = []() {return static_cast<RecordingHelper*>(&recorder)->IsRecordingSVO(); };

	//float(*speedgetter)() = []() {return Config::forwardSpeedMPS(); };

	//bool(*recordgetter)() = []() {return static_cast<RecordingHelper*>(record)->IsRecordingSVO(); };
	//auto recordgetter = [&recorder]() {return recorder.IsRecordingSVO(); };

	//ToggleButton_Record recordButton(recordinggetter, recordingsetter, 0, 1, 0, 1);

	// Loop until 'q' is pressed
	char key = ' ';
	while (key != 'q') {

		if (zed.grab(runtime_parameters) == ERROR_CODE::SUCCESS) {

			//Log a new frame in TimeHelper, so that we can accurately get deltaTime later. 
			Time::LogNewFrame();

			//TEMP: Start recording if the R key is pressed. 
			if (key == 'r')
			{
				if (!recorder.IsRecordingSVO()) recorder.StartRecording();
				else recorder.StopRecording();

			}

			//cout << Time::fps() << endl;

			// Retrieve the left image, depth image in half-resolution
			zed.retrieveImage(zedimage, VIEW::LEFT, MEM::CPU, image_size);

			zed.retrieveMeasure(depth_measure);

			zed.getSensorsData(sensorData, TIME_REFERENCE::IMAGE);


			//Make the UI image. 
			int screenrot = Config::screenRotation();
			int uiwidth = (screenrot % 2 == 0) ? image_ocv.cols : image_ocv.rows;
			int uiheight = (screenrot % 2 == 0) ? image_ocv.rows : image_ocv.cols;
			ui_mat = cv::Mat(uiheight, uiwidth, CV_8UC4);
			ui_mat.setTo(cv::Scalar(0, 0, 0, 0));

			//Draw the FPS in the top right. 
			char fpsbuffer[5];
			int n = sprintf(fpsbuffer, "%1.0f", Time::smoothedFPS());
			cv::Size fpssize = cv::getTextSize(fpsbuffer, 1, 1, 1, NULL);
			cv::putText(ui_mat, fpsbuffer, cv::Point(uiwidth - fpssize.width - 2, fpssize.height + 2), 1, 1, cv::Scalar(102, 204, 0, 100));



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
					float dotradius = 15 / collisiondepth_nograv;
					if (dotradius <= 0)
					{
						cout << "Dotradius was less than zero: " << dotradius << " Depth: " << collisiondepth_nograv << endl;
						dotradius = 2.0;
					}
					//cout << "Collided at " << collisionpoint_nograv.x << ", " << collisionpoint_nograv.y << endl;
					cv::circle(image_ocv, cv::Point(collisionpoint_nograv.x, collisionpoint_nograv.y), dotradius, cv::Scalar(0, 255.0, 0), -1);
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
					float dotradius = 15 / collisiondepth_grav;
					if (dotradius <= 0)
					{
						cout << "Dotradius was less than zero: " << dotradius << " Depth: " << collisiondepth_grav << endl;
						dotradius = 2.0;
					}

					//cout << "Collided at " << collisionpoint_grav.x << ", " << collisionpoint_grav.y << endl;
					cv::circle(image_ocv, cv::Point(collisionpoint_grav.x, collisionpoint_grav.y), dotradius, cv::Scalar(0, 0, 255.0, 1), -1);
					
					//Draw the distance and travel time. 
					//If statement is to save calculating the rotated point that the text is anchored to. 
					if (Config::toggleDistance() || Config::toggleTravelTime()) 
					{
						cv::Point gravpoint_ui = imageHelper.RawImagePointToRotated(cv::Point(collisionpoint_grav.x, collisionpoint_grav.y));

						if (Config::toggleDistance())
						{
							char distbuffer[20];
							int d = sprintf(distbuffer, "%0.2f", collisiondepth_grav);
							string distsuffix = "m";
							string disttext = distbuffer + distsuffix; //Not sure why I can't just add the literal. 
							cv::Size disttextsize = cv::getTextSize(disttext, 1, 1, 1, NULL);

							cv::Point disttextpoint(gravpoint_ui.x - 20 - dotradius - disttextsize.width,
								gravpoint_ui.y - 20 - dotradius);

							cv::putText(ui_mat, disttext, disttextpoint, 1, 1, cv::Scalar(0, 0, 255, 1));
						}
						if (Config::toggleTravelTime())
						{
							char timebuffer[20];
							int t = sprintf(timebuffer, "%0.2f", traveltime_grav);
							string timesuffix = "s";
							string timetext = timebuffer + timesuffix;

							cv::Point timetextpoint(gravpoint_ui.x + 20 + dotradius,
								gravpoint_ui.y - 20 - dotradius);


							cv::putText(ui_mat, timetext, timetextpoint, 1, 1, cv::Scalar(0, 0, 255, 1));
						}
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
			//panelrect = cv::Rect(2, 1, ui_mat.cols - 2, ui_mat.rows * 0.2 - 2);
			panelrect = cv::Rect(0, 0, 50, 300);

			//Calculate the menu size and assign it. 
			cv::Rect menurect = cv::Rect(panelrect.width, 0, zedheight - panelrect.width, (zedheight - panelrect.width) / 3.0);
			panel.openPanelRect = menurect;
			
			panel.ProcessUI(panelrect, ui_mat, "EasiAug");

			//Draw the recording button. 
			float buttondim = ui_mat.cols / 15.0;
			cv::Rect recordbuttonrect = cv::Rect(ui_mat.cols - buttondim, ui_mat.rows - buttondim, buttondim, buttondim);
			recordButton.ProcessUI(recordbuttonrect, ui_mat, "EasiAug");

			//Rotate the image. 
			cv::Mat finalimagemat;
			finalimagemat = imageHelper.RotateImageToConfig(image_ocv);
			
			//Rotate the UI image. 
			cv::Mat finaluimat = imageHelper.RotateUIToConfig(ui_mat);

			//Add the UI image to it. 
			cv::Mat mask(cv::Size(finaluimat.cols, finaluimat.rows), CV_8UC1);
			int fromto[] = { 3, 0 };
			cv::mixChannels(finaluimat, mask, fromto, 1);
			cv::subtract(finalimagemat, cv::Scalar(255, 255, 255, 255), finalimagemat, mask);
			cv::add(finalimagemat, finaluimat, finalimagemat, mask);
		

			cv::imshow("EasiAug", finalimagemat);


			//Input.
			cv::setMouseCallback("EasiAug", ClickCallback, &imageHelper);


			// Handle key event
			key = cv::waitKey(10);
		}
	}
	recorder.StopRecording(); //If we're recording, close it out. 
	zed.close();
	return 0;
}

bool GetIsRecording()
{
	return recordHelper->IsRecordingSVO();
}

//void ClickCallback(int event, int x, int y, int flags, void* userdata)
void ClickCallback(int event, int x, int y, int flags, void* userdata)
{
	if (event == CV_EVENT_LBUTTONDOWN || event == CV_EVENT_LBUTTONUP)
	{
		//cout << "Before: X: " << x << " Y: " << y << endl;

		//Clickable::ProcessAllClicks(x, y);
		ImageHelper *imageHelper = static_cast<ImageHelper*>(userdata);
		//int2 rotateduipoints = imageHelper->ScreenTouchToUIPoint(x, y);
		imageHelper->ScreenTouchToUIPoint(&x, &y);
		//int2 rotateduipoints = int2(x, y);

		bool isdown = event == CV_EVENT_LBUTTONDOWN; 

		//cout << "After: X: " << x << " Y: " << y << endl;

		//panel.ProcessAllClicks(rotateduipoints.x, rotateduipoints.y, isdown);
		panel.ProcessAllClicks(x, y, isdown);
		recordButton.ProcessAllClicks(x, y, isdown);
		//panel.ProcessAllClicks(x, y);
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


