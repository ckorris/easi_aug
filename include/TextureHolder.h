#pragma once

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>


class TextureHolder
{
public:
	TextureHolder::TextureHolder(sl::Mat *zedImage, sl::Mat *depthMeasure, cv::Mat *ocvImage);
	
	void CreateMatrixesFromZed(sl::Camera *zed);


private:
	bool _isSetUp;
	
	sl::Mat *_zedImage;
	sl::Mat *_depthMeasure;
	cv::Mat *_ocvImage;

};