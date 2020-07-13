#include <SPIOutputHelper.h>
//#include <SPIScreen.h>

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <unistd.h>
#include <functional>

using namespace std;


mutex mx;
bool newImage;
bool quit;
cv::Mat bufferMat;

thread spiThread;

SPIOutputHelper::SPIOutputHelper()
{
	//screen = SPIScreen;
	newImage = false;
	quit = false;

	spiThread = thread(bind(&SPIOutputHelper::ThreadedLoop, this));
}

SPIOutputHelper::~SPIOutputHelper()
{
	quit = true;

	spiThread.join();
}

void SPIOutputHelper::ThreadedLoop()
{
	//This will live entirely on this thread so we don't have pin memory conflicts. 
	SPIScreen screen;

	//Should be fine to check bools outside a lock, but come back to that if we have issues. 
	while (quit == false)
	{
		if(newImage == true)
		{
			mx.lock();
			
			//Copy the buffer to a new image and remove the alpha at once. 
			cv::Mat noalphamat;
			cv::cvtColor(bufferMat, noalphamat, cv::COLOR_BGRA2BGR);
			
			newImage = false;
			mx.unlock();
			
			//Convert to an unsigned char vector for passing to SPIScreen.
			vector<unsigned char> imgvec = ImageToVector(noalphamat);
			
			//Send to the LCD screen. 
			screen.LCD_ShowImage(imgvec, noalphamat.cols, noalphamat.rows, noalphamat.channels());
		}
		else
		{
			usleep(200); //0.2 milliseconds. 
		}
	}
}

void SPIOutputHelper::DisplayImageOnSPIScreen(cv::Mat image)
{
	//Pass the image to the buffer and let the other thread know it's got mail. 
	mx.lock();
	//Scale the image and copy it to the threaded buffer at once. 
	cv::resize(image, bufferMat, cv::Size(320, 240));
	newImage = true;
	mx.unlock();

	/*
	//TO DO: Consider multi-threading, and if this gets too complex, 
	//move to a different class. 
	//Make a new Mat, both to resize it and act as a buffer. 
	//cv::Mat scaledimg = cv::Mat(240, 320, CV_8UC3);
	cv::Mat scaledimg;	
	cv::resize(image, scaledimg, cv::Size(320, 240));

	cv::Mat scaledimg_noalpha = (cv::Mat(320, 240, CV_8UC3));
	cv::cvtColor(scaledimg, scaledimg_noalpha, cv::COLOR_BGRA2BGR);

	//vector<unsigned char> scaledimgvec = ImageToVector(scaledimg);
	vector<unsigned char> scaledimgvec = ImageToVector(scaledimg_noalpha);

	screen.LCD_ShowImage(scaledimgvec, scaledimg_noalpha.cols, scaledimg_noalpha.rows, scaledimg_noalpha.channels());
	*/
}

void ProcessImage(cv::Mat inputmat) //Kinda redundant, should probably delete.
{
	mx.lock();
	//Scale the image and copy it to the threaded buffer at once. 
	cv::resize(inputmat, bufferMat, cv::Size(320, 240));
	newImage = true;
	mx.unlock();
}





vector<unsigned char> SPIOutputHelper::ImageToVector(cv::Mat image)
{
	//Try reading as array of uchars. 
	vector<unsigned char> imgarray(image.rows * image.cols * image.channels());
	//Assuming the Mat is continuous. 
	if(image.isContinuous() == false)
	{
		cout << "Error: Image not continuous." << endl;
	}

	imgarray.assign(image.data, image.data + image.total() * image.channels());

	return imgarray;
}

cv::Mat SPIOutputHelper::LoadImage(string path) //For testing, really. 
{
	cv::Mat image;
	image = cv::imread(path, 1);

	if(!image.data)
	{
		cout << "Error reading image." << endl;
	}

	cout << "Image type: " << image.type();

	return image;
}




