#include <SPIOutputHelper.h>
//#include <SPIScreen.h>

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>


using namespace std;

SPIScreen screen;

SPIOutputHelper::SPIOutputHelper()
{
	//screen = SPIScreen;
}

SPIOutputHelper::~SPIOutputHelper()
{
	
}

void SPIOutputHelper::DisplayImageOnSPIScreen(cv::Mat image)
{
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




