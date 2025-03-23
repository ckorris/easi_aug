#include <sl/Camera.hpp>

#include <opencv2/opencv.hpp>
#include <external/Simulation.h>
#include <CamUtilities.h>
#include <Config.h>
#include <Drawables.h>
#include <ImageHelper.h>
#include <TimeHelper.h>
#include <RecordingHelper.h>
#include <IOShortcuts.h>
#include <TextureHolder.h>
#include <HotkeyManager.h>

#if SPI_OUTPUT
#include <SPIOutputHelper.h>
#endif
using namespace hps;

using namespace std;
using namespace sl;

const bool SET_TO_FULL_SCREEN = false; //If true, will make window full-screen. Best for small HDMI screens.

//Variables.
cv::Point mousePos(0,0); //Current position of the mouse. Updated within ClickCallback (okay so I need to rename that). 
//bool zoom = false; //When true, the images will be magnified by 2x.

//Non-ZED helpers/managers.
Simulation *sim;
ImageHelper *imageHelper;
RecordingHelper *recorder;

RecordingHelper* recordHelper; //Not actually used. //Update: TODO: Figure out why there are two of this now. There was a reason that I failed to document.
bool(*recordinggetter)() = []() { return recordHelper->IsRecordingSVO(); };
void(*recordingsetter)(bool) = [](bool v) { recordHelper->ToggleRecording(v); };

//UI.
cv::Rect tempmenurect(50, 0, 450, 180);
Sidebar panel(tempmenurect, 0, 1, 0, 1);
ToggleButton_Record recordButton(recordinggetter, recordingsetter, 0, 1, 0, 1);

//ZED-specific fields/properties.
//Mat zedimage;
//Mat *depth_measure;
SensorsData sensorData;
RuntimeParameters runtime_parameters;
Resolution image_size;

TextureHolder *textureHolder;

bool wantsToQuit = false;

//cv::Mat image_ocv;

int zedWidth()
{
	return image_size.width;
}
int zedHeight()
{
	return image_size.height;
}

//Forward declarations.
cv::Mat slMat2cvMat(Mat& input);
int slMatType2cvMatType(sl::MAT_TYPE sltype);
void ClickCallback(int event, int x, int y, int flags, void* userdata);
void DrawUI(int uiwidth, int uiheight, cv::Mat *outMat);
void DrawSimulation(int uiwidth, int uiheight, Mat depth_measure, cv::Mat projectionMatrix, 
	cv::Mat image_ocv, cv::Mat *outSimMat);
bool DetectCollision(const hps::float3& lastValidPoint, const hps::float3& currentPoint);
void CombineIntoFinalImage(int uiwidth, int uiheight, cv::Mat image_ocv, cv::Mat sim_mat, cv::Mat menu_mat, 
	cv::Mat *finalMat);
void HandleOutputAndMouse(cv::Mat finalImageMat);
void RequestClose();

Camera zed;

Mat depth_measure;
cv::Mat projectionMatrix;

int main(int argc, char **argv) 
{
#if SPI_OUTPUT
	//Create object for outputting to SPI monitor.
	SPIOutputHelper spihelper;

#endif

	//Initialize the ZED.
	int result = 0;
	result = CamUtilities::InitZed(argc, argv, &zed, &runtime_parameters, &image_size);


	if (result == -1)
	{
		return -1;
	}

	// Prepare new images to be used as the background, or video of the real world in both formats.
	Mat zedimage;
	cv::Mat image_ocv;



	TextureHolder texholder(&zedimage, &depth_measure, &image_ocv);
	textureHolder = &texholder;
	textureHolder->CreateMatrixesFromZed(&zed);
	
	Simulation simReal = Simulation();
	sim = &simReal;

	//imageHelper = &ImageHelper(&zed);
	imageHelper =  new ImageHelper(&zed);

	sl::CameraInformation info = zed.getCameraInformation();
	projectionMatrix = CamUtilities::GetProjectionMatrix(info);

	//Declare the recording helper (for recording SVO files) and button.
	//recorder = &RecordingHelper(&zed);
	recorder =  new RecordingHelper(&zed);
	RecordingHelper record = static_cast<RecordingHelper>(*recorder);
	recordHelper = recorder;

	//TODO: I HATE having to pass an explicit reference to RecordingHelper to the Ui. 
	//That means the UI has to know what a RecordingHelper is. But passing a reference to it is proving very hard. 
	//Find a way around that that's not messy. 
	bool(*recordinggetter)() = []() { return recordHelper->IsRecordingSVO(); };
	void(*recordingsetter)(bool) = [](bool v) { recordHelper->ToggleRecording(v); };

	//Make the hotkey manager and assign hotkeys.
	HotkeyManager hotkeyManager;
	hotkeyManager.RegisterKeyBinding('z', IOShortcuts::IncrementZoom);
	hotkeyManager.RegisterKeyBinding('x', IOShortcuts::ToggleSimulationOverlay);
	hotkeyManager.RegisterKeyBinding('c', []() { IOShortcuts::IncrementResolution(&zed, &runtime_parameters, &image_size, textureHolder); });
	hotkeyManager.RegisterKeyBinding('v', []() { IOShortcuts::SleepMode(&zed); });
	hotkeyManager.RegisterKeyBinding('q', RequestClose);

	// Loop until 'q' is pressed
	char key = ' ';
	while (wantsToQuit == false) {
		
		if (zed.grab(runtime_parameters) == ERROR_CODE::SUCCESS) 
		{
			//Log a new frame in TimeHelper, so that we can accurately get deltaTime later. 
			Time::LogNewFrame();

			//Retrieve data from the ZED.
			zed.retrieveImage(zedimage, VIEW::LEFT, MEM::CPU, image_size);
			zed.retrieveMeasure(depth_measure);
			zed.getSensorsData(sensorData, TIME_REFERENCE::IMAGE);

			//Calculate the ui dimensions based on the screen rotation.
			int screenrot = Config::screenRotation();
			int uiwidth = (screenrot % 2 == 0) ? image_ocv.cols : image_ocv.rows;
			int uiheight = (screenrot % 2 == 0) ? image_ocv.rows : image_ocv.cols;

			//Run the simulation and draw the relevant images onto the relevant mats.
			cv::Mat sim_mat;
			DrawSimulation(uiwidth, uiheight, depth_measure, projectionMatrix, image_ocv, &sim_mat);

			//Draw the UI into a mat.
			cv::Mat menu_mat;
			DrawUI(uiwidth, uiheight, &menu_mat);

			cv::Mat finalImageMat;
			CombineIntoFinalImage(uiwidth, uiheight, image_ocv, sim_mat, menu_mat, &finalImageMat);

			//Display on the window and/or LCD screen, and handle mouse drawing and input.
			HandleOutputAndMouse(finalImageMat);
		
			//Process hotkeys.
			hotkeyManager.Process();

			// Handle key event
			//key = cv::waitKey(10);
		}
	}
	recorder->StopRecording(); //If we're recording, close it out. 
	zed.close();
	return 0;
}

void DrawUI(int uiwidth, int uiheight, cv::Mat *outMat)
{
	//Make the menu image. Note this behaves very similarly to sim_mat, but is kept separate so we can add an offset at the end. 
	cv::Mat menu_mat = cv::Mat(uiheight, uiwidth, CV_8UC4);
	menu_mat.setTo(cv::Scalar(0, 0, 0, 0));

	//Width and height of the final output screen. 
	int destwidth = Config::lcdWidth();
	int destheight = Config::lcdHeight();

	//Make sure those values aren't zero.
	if (destwidth == 0 || destheight == 0)
	{
		cout << "lcdWidth and/or lcdHeight were set to zero. Defaulting to ZED resolution. Do you have iLCDWidth and iLCDHeight set in your config file?" << endl;
		destwidth = zedWidth();
		destheight = zedHeight();
	}

	//Calculate the menu size and assign it. 
	//This involves figuring out what the image will be scaled to, and what will be cropped for the output res. 	
	float srcaspect = menu_mat.cols / (float)menu_mat.rows;
	float destaspect = destwidth / (float)destheight;

	//cout << "Converting aspects. Src: " << srcaspect << " Dst: " << destaspect << endl;
	float widthmult = destaspect / srcaspect;
	float heightmult = srcaspect / destaspect;

	//Clamp the above between 0 and 1. (Apparently clamp isn't in this version of C++?)
	if (widthmult > 1.0) widthmult = 1.0;
	if (heightmult > 1.0) heightmult = 1.0;

	float croppedwidth = menu_mat.cols * widthmult;
	float croppedheight = menu_mat.rows * heightmult;

	float widthdiff = menu_mat.cols - croppedwidth;
	float heightdiff = menu_mat.rows - croppedheight;

	cv::Rect panelrect; //Rect of the sidebar. The rect for the rest of the menu will be set partially based on these values, too.
	//panelrect = cv::Rect(2, 1, menu_mat.cols - 2, menu_mat.rows * 0.2 - 2);
	panelrect = cv::Rect((widthdiff / 2), (heightdiff / 2), 50, 300);

	cv::Rect menurect = cv::Rect(panelrect.x + panelrect.width, panelrect.y, zedHeight() - panelrect.width, (zedHeight() - panelrect.width) / 3.0);
	panel.openPanelRect = menurect;

	panel.ProcessUI(panelrect, menu_mat, "EasiAug");


	//Draw the recording button. 
	float buttondim = menu_mat.cols / 15.0;
	cv::Rect recordbuttonrect = cv::Rect(menu_mat.cols - (widthdiff / 2) - buttondim, menu_mat.rows - (heightdiff / 2) - buttondim, buttondim, buttondim);
	recordButton.ProcessUI(recordbuttonrect, menu_mat, "EasiAug");

	//Draw the FPS in the top right. 
	char fpsbuffer[5];
	int n = sprintf(fpsbuffer, "%1.0f", Time::smoothedFPS());
	//cv::Size fpssize = cv::getTextSize(fpsbuffer, 1, 1, 1, NULL); //Normal.
	cv::Size fpssize = cv::getTextSize(fpsbuffer, 1, 2, 2, NULL); //Double.
	//cv::putText(sim_mat, fpsbuffer, cv::Point(uiwidth - fpssize.width - 2, fpssize.height + 2), 1, 1, cv::Scalar(102, 204, 0, 100)); //Normal.
	cv::putText(menu_mat, fpsbuffer, cv::Point(menu_mat.cols - (widthdiff / 2) - fpssize.width - 2,
		fpssize.height + (heightdiff / 2) + 2), 1, 2, cv::Scalar(102, 204, 0, 100), 2);



	*outMat = menu_mat;
}

void DrawSimulation(int uiwidth, int uiheight, Mat depth_measure, cv::Mat projectionMatrix, 
					cv::Mat image_ocv, cv::Mat *outSimMat)
{//Make the image used for some displays, like the distance text.
	cv::Mat sim_mat = cv::Mat(uiheight, uiwidth, CV_8UC4);
	sim_mat.setTo(cv::Scalar(0, 0, 0, 0)); //Maaaay not need, now that we're drawing the menu separately. 

	//Laser crosshair - no gravity. 
	if (Config::toggleLaserCrosshair() || Config::toggleLaserPath())
	{
		//TODO: Make this work.
		/*
		bool collided = sim->Simulate(traveltime_nograv, maxSamples, camPosOffset, camRotOffset, physicsArgs,
			gravityVector, collisionDetectionFunc, collisionDepth, totalTime, linePoints, sampleStats, stats);

		//bool collided = sim->Simulate(depth_measure, Config::forwardSpeedMPS(), 0.04, false, sensorData, collisionpoint_nograv, collisiondepth_nograv, 
		//	traveltime_nograv, Config::toggleLaserPath(), image_ocv, cv::Scalar(0, 255.0, 0, 1));
		
		
		if (collided && Config::toggleLaserCrosshair())
		{
			float dotradius = 15 / collisiondepth_nograv;
			if (dotradius <= 0)
			{
				cout << "Dotradius was less than zero: " << dotradius << " Depth: " << collisiondepth_nograv << endl;
				dotradius = 2.0;
			}

			cv::circle(image_ocv, cv::Point(collisionpoint_nograv.x, collisionpoint_nograv.y), dotradius, cv::Scalar(0, 255.0, 0), -1);
		}
		*/
	}

	//Laser crosshair - gravity.
	if (Config::toggleGravityCrosshair() || Config::toggleGravityPath())
	{
		
		

		//TEMP consts that should be messed with and made real consts.
		int maxSamples = 100;
		float sampleTime = 0.01f;

		hps::float3 camPosOffset(0, 0, 0);
		camPosOffset.x = Config::camXPos();
		camPosOffset.y = Config::camYPos();
		camPosOffset.z = Config::camZPos();

		hps::float3 camRotOffset(Config::camXRot(), Config::camYRot(), Config::camZRot());

		hps::PhysicsArgs physicsArgs;
		physicsArgs.BBDiameterMM = Config::bbDiameterMM();
		physicsArgs.BBMassGrams = Config::bbMassGrams();
		physicsArgs.StartSpeedMPS = Config::forwardSpeedMPS();
		physicsArgs.SpinRPM = Config::hopUpRPM();
		physicsArgs.TemperatureCelsius = Config::temperatureC();
		physicsArgs.RelativeHumidity01 = Config::relativeHumidity01();
		physicsArgs.PressureHPa = sensorData.barometer.pressure;
		physicsArgs.BBToAirFrictionCoefficient = Config::bbToAirFrictionCoef();

		sl::float3 gravityVectorSL = sensorData.imu.linear_acceleration;

		//Convert to custom format, and invert X to handle handedness.
		hps::float3 gravityVector = hps::float3(-gravityVectorSL.x, gravityVectorSL.y, gravityVectorSL.z);

		//Get the collision function, which looks up the depth values to decide if there was a collision or not.

		CollisionDetectionFunc collisionDetectionFunc = [](const hps::float3& lastValidPoint, const hps::float3& currentPoint) ->
			bool {

			return DetectCollision(lastValidPoint, currentPoint);
			};
		
		float collisionDepth;
		float totalTime;
		int linePointsCount;

		std::vector<hps::float3> linePoints;

		//SampleStats** sampleStats;
		std::vector<SampleStats> sampleStats;
		SimStats stats;
		
		bool collided = sim->Simulate(sampleTime, maxSamples, camPosOffset, camRotOffset, physicsArgs,
			gravityVector, collisionDetectionFunc, collisionDepth, totalTime, linePoints, sampleStats, stats);
		
		if (linePoints.size() == 0)
		{
			*outSimMat = sim_mat;
			return;
		}
		
		float depthWidth = (int)depth_measure.getWidth();
		float depthHeight = (int)depth_measure.getHeight();
		
		sl::float3 firstPointSL = sl::float3(camPosOffset.x, camPosOffset.y, 0);
				
		sl::int2 lastScreenPos = CamUtilities::CameraToScreenPos(firstPointSL, projectionMatrix, depthWidth, depthHeight);

		for(size_t i = 0; i < linePoints.size() ; i++) //Start on the second, so the last one is the first one.
		{
			hps::float3 currentPoint = linePoints[i];
			sl::float3 currentPointSL = sl::float3(currentPoint.x, currentPoint.y, currentPoint.z);

			sl::int2 currentScreenPos = CamUtilities::CameraToScreenPos(currentPointSL, projectionMatrix, depthWidth, depthHeight);
			
			if (lastScreenPos.x != currentScreenPos.x || lastScreenPos.y != currentScreenPos.y) //Comparing int2 to int2 causes weird errors.
			{
				//Due to how this works, we're transforming the points twice, once during collision, once here.
				//TODO: Optimize.
				cv::Scalar color = cv::Scalar(0, 0, 255.0, 1);

				cv::line(image_ocv, cv::Point(lastScreenPos.x, lastScreenPos.y), cv::Point(currentScreenPos.x, currentScreenPos.y), 
					color, 2);

				lastScreenPos = currentScreenPos;
			}
		}
		
		hps::int2 collisionpoint_grav = hps::int2(lastScreenPos.x, lastScreenPos.y);
		float collisiondepth_grav = linePoints[linePoints.size() - 1].z; 
		float traveltime_grav = sampleTime * linePoints.size();

		//bool collided = sim->Simulate(depth_measure, Config::forwardSpeedMPS(), 0.04, true, sensorData, collisionpoint_grav, collisiondepth_grav, traveltime_grav,
		//	Config::toggleGravityPath(), image_ocv, cv::Scalar(0, 0, 255.0, 1));
		
		if (collided && Config::toggleGravityCrosshair())
		{
			float dotradius = 15 / collisiondepth_grav;
			if (dotradius <= 0)
			{
				cout << "Dotradius was less than zero: " << dotradius << " Depth: " << collisiondepth_grav << endl;
				dotradius = 2.0;
			}

			cv::circle(image_ocv, cv::Point(collisionpoint_grav.x, collisionpoint_grav.y), dotradius, cv::Scalar(0, 0, 255.0, 1), -1);

			//Draw the distance and travel time. 
			//If statement is to save calculating the rotated point that the text is anchored to. 
			if (Config::toggleDistance() || Config::toggleTravelTime())
			{
				cv::Point gravpoint_ui = imageHelper->RawImagePointToRotated(cv::Point(collisionpoint_grav.x, collisionpoint_grav.y));

				int zoom = Config::zoomLevel();

				if (Config::toggleDistance())
				{
					char distbuffer[20];
					int d = sprintf(distbuffer, "%0.2f", collisiondepth_grav);
					string distsuffix = "m";
					string disttext = distbuffer + distsuffix; //Not sure why I can't just add the literal. 

					//Draw text with double size.
					cv::Size disttextsize = cv::getTextSize(disttext, 1, zoom, zoom, NULL);
					cv::Point disttextpoint(gravpoint_ui.x - 20 - dotradius - disttextsize.width,
						gravpoint_ui.y - 20 - dotradius);

					cv::putText(sim_mat, disttext, disttextpoint, 1, zoom, cv::Scalar(0, 0, 255, 1), zoom);
				}
				if (Config::toggleTravelTime())
				{
					char timebuffer[20];
					int t = sprintf(timebuffer, "%0.2f", traveltime_grav);
					string timesuffix = "s";
					string timetext = timebuffer + timesuffix;

					cv::Point timetextpoint(gravpoint_ui.x + 20 + dotradius,
						gravpoint_ui.y - 20 - dotradius);

					cv::putText(sim_mat, timetext, timetextpoint, 1, zoom, cv::Scalar(0, 0, 255, 1), zoom);
				}
			}
		}
		else
		{
			//cout << "Did not collide. Last was " << collisionpoint_grav.x << ", " << collisionpoint_grav.y << endl;
		}
		
	}

	*outSimMat = sim_mat;
}

bool DetectCollision(const hps::float3& lastValidPoint, const hps::float3& currentPoint)
{
	//return false;
	
	//For simplicity, just check the current position against the depth of its corresponding point
	//in the ZED depth image. We could be fancier and check for collision along all the 2D points between
	//the last and current one, but this would very rarely make a difference, and it would be slight,
	//for a sizeable performance drop.

	if (currentPoint.z < 0) //If behind the camera, don't test.
	{
		return false; 
	}

	int imageWidth = (int)depth_measure.getWidth();
	int imageHeight = (int)depth_measure.getHeight();

	sl::float3 currentPointSL = sl::float3(currentPoint.x, currentPoint.y, currentPoint.z);

	//hps::int2 screenPosition = CamUtilities::CameraToScreenPos(currentPointSL, projectionMatrix,
	sl::int2 screenPosition = CamUtilities::CameraToScreenPos(currentPointSL, projectionMatrix,
		imageWidth, imageHeight);

	//If it's outside view of the screen, we won't be able to calculate depth. 
	if (screenPosition.x < 0.0 || screenPosition.y < 0.0 || 
		screenPosition.x > imageWidth || screenPosition.y > imageHeight)
	{
		return false;
	}

	float zedDepth;
	depth_measure.getValue(screenPosition.x, screenPosition.y, &zedDepth);
	
	return zedDepth > 0.0 && currentPoint.z > zedDepth; //Current point is behind the depth of the image.

}

void CombineIntoFinalImage(int uiwidth, int uiheight, cv::Mat image_ocv, cv::Mat sim_mat, cv::Mat menu_mat, cv::Mat *finalMat)
{
	//Rotate the ZED image, and possibly zoom in.  
	cv::Mat finalimagemat(image_ocv.cols, image_ocv.rows, CV_8UC4);
	cv::Mat finaluimat(sim_mat.cols, sim_mat.rows, CV_8UC4);
	int zoom = Config::zoomLevel();
	if (zoom == 1) //Save some maths and some cycles. 
	{
		finalimagemat = imageHelper->RotateImageToConfig(image_ocv);
		finaluimat = imageHelper->RotateUIToConfig(sim_mat);
	}
	else //zoom != 1
	{
		cv::Mat rotatedzedimage = imageHelper->RotateImageToConfig(image_ocv);
		cv::Mat rotateduiimage = imageHelper->RotateUIToConfig(sim_mat);

		float sidelength = (1.0 - (1.0 / zoom)) / 2.0;
		//cv::Rect zoomrect(image_ocv.cols / 4.0, image_ocv.rows / 4.0, image_ocv.cols / 2.0, image_ocv.rows / 2.0);
		cv::Rect zoomrect(image_ocv.cols * sidelength, image_ocv.rows * sidelength, image_ocv.cols / zoom, image_ocv.rows / zoom);

		cv::resize(rotatedzedimage(zoomrect), finalimagemat, cv::Size(uiwidth, uiheight));
		cv::resize(rotateduiimage(zoomrect), finaluimat, cv::Size(uiwidth, uiheight));
	}

	//Declare a "fromto" array to be used in several additive functions (cv::mixChannels). 
	int fromto[] = { 3, 0 };

	//Rotate the UI mat and add the menu mat to the sim mat. 
	cv::Mat menumask(cv::Size(menu_mat.cols, menu_mat.rows), CV_8UC1);
	cv::mixChannels(menu_mat, menumask, fromto, 1);
	cv::add(finaluimat, menu_mat, finaluimat, menumask);


	//Add the UI image (now with the menu) to the ZED image. 
	cv::Mat mask(cv::Size(finaluimat.cols, finaluimat.rows), CV_8UC1);
	cv::mixChannels(finaluimat, mask, fromto, 1);
	cv::subtract(finalimagemat, cv::Scalar(255, 255, 255, 255), finalimagemat, mask);
	cv::add(finalimagemat, finaluimat, finalimagemat, mask);

	*finalMat = finalimagemat;
}

void HandleOutputAndMouse(cv::Mat finalImageMat)
{
	//Draw the mouse.
	cv::circle(finalImageMat, mousePos, (finalImageMat.cols / 150), cv::Scalar(0, 255, 255, 255), -1);

	//Output to desktop.
	if (SET_TO_FULL_SCREEN == true)
	{
		cv::namedWindow("EasiAug", cv::WINDOW_KEEPRATIO);
		cv::setWindowProperty("EasiAug", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
	}
	else
	{
		cv::namedWindow("EasiAug");
	}

	cv::imshow("EasiAug", finalImageMat);

#if SPI_OUTPUT
	//Output to SPI screen.
	spihelper.DisplayImageOnSPIScreen(finalimagemat);
#endif

	//Mouse input.
	cv::setMouseCallback("EasiAug", ClickCallback, &imageHelper);
}

bool GetIsRecording()
{
	return recordHelper->IsRecordingSVO();
}

void ClickCallback(int event, int x, int y, int flags, void* userdata)
{
	
	if (event == cv::EVENT_LBUTTONDOWN || event == cv::EVENT_LBUTTONUP)
	{
		ImageHelper *imageHelper = static_cast<ImageHelper*>(userdata);
		imageHelper->ScreenTouchToUIPoint(&x, &y);

		bool isdown = event == cv::EVENT_LBUTTONDOWN; 

		panel.ProcessAllClicks(x, y, isdown);
		recordButton.ProcessAllClicks(x, y, isdown);
	}
	else if (event == cv::EVENT_RBUTTONDOWN)
	{
		//IOShortcuts::IncrementResolution(&zed, &runtime_parameters, &image_size, textureHolder);
		IOShortcuts::IncrementZoom();
	}
	else if (event == cv::EVENT_MOUSEMOVE)
	{
		mousePos = cv::Point(x, y);
	}
	
}

void RequestClose()
{
	wantsToQuit = true;
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


