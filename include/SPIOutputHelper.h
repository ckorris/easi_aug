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
	cv::Mat LoadImage(string path);
	vector<unsigned char> ImageToVector(cv::Mat image);
	
	//SPIScreen screen;
};
