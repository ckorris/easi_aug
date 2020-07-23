#if SPI_OUTPUT

#include <SPIScreen.h>

#include <opencv2/opencv.hpp>
#include <vector>


using namespace std;

class SPIOutputHelper
{
public:
	SPIOutputHelper();
	~SPIOutputHelper();
	
	void DisplayImageOnSPIScreen(cv::Mat image);

private: 
	
	vector<unsigned char> ImageToVector(cv::Mat image);
	cv::Mat LoadImage(string path);
	void ThreadedLoop();
	void ProcessImage(cv::Mat inputmat);	

	cv::Mat bufferMat;
	//SPIScreen screen;
};

#endif