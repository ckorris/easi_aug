#include <ImageHelper.h>

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

#include <Config.h>

ImageHelper::ImageHelper(sl::Camera *zedref)
{
	zed = zedref;
}

float ImageHelper::GetRotationScale()
{
	int imagerotamount = Config::imageRotation();

	//TEST
	//return 1.0;

	//If we're not rotating at all, or rotating 180°, we don't have to do any scaling. 
	if (imagerotamount % 2 == 0)
	{
		return 1.0;
	}
	else //Otherwise, unscaled, the height of the image won't be big enough to fit the width of the screen, so scale accordingly.
	{
		sl::Resolution zedres = zed->getCameraInformation().camera_configuration.resolution;

		return zedres.width / (float)zedres.height;
	}
}

//Rotates and scales the image to fit the desired image rotation specified in Config while filling the output window.
cv::Mat ImageHelper::RotateImageToConfig(cv::Mat unrotatedmat)
{
	int imagerotamount = Config::imageRotation();

	if (imagerotamount == 0)
	{
		return unrotatedmat; //It doesn't need to be rotated. Don't change anything. 
	}

	//We'll performt the rotation into a buffer texture, then assign it at the end. 
	sl::Resolution zedres = zed->getCameraInformation().camera_configuration.resolution;
	cv::Mat rotatedmat;
	cv::Point2d centerpoint = cv::Point2d(zedres.width / 2, zedres.height / 2);

	if (imagerotamount == 1)
	{
		rotatedmat = cv::Mat(cv::Size(zedres.height, zedres.width), CV_8UC4);
	}
	else if (imagerotamount == 2) //Same dimensions as input since we just flipped it upside down. 
	{
		rotatedmat = cv::Mat(cv::Size(zedres.width, zedres.height), CV_8UC4);
	}
	else if (imagerotamount == 3)
	{
		rotatedmat = cv::Mat(cv::Size(zedres.height, zedres.width), CV_8UC4); //Currently duplicate of == 1. 
	}
	else
	{
		cout << "Tried to set image rotation based on invalid height " << imagerotamount << ". Should be 0-3." << endl;
		return unrotatedmat;
	}

	float rotangle = imagerotamount * 90.0;
	cv::Mat imagerotmatrix = cv::getRotationMatrix2D(centerpoint, -rotangle, GetRotationScale());
	cv::warpAffine(unrotatedmat, rotatedmat, imagerotmatrix, unrotatedmat.size());

	return rotatedmat;

}

///Rotates the UI mat to fit the screen as intended. Assumes the mat was created with a size based on the rotation, ie, 
///if rotated 90° or 270°, the width and height are reversed (but not in the output). 
cv::Mat ImageHelper::RotateUIToConfig(cv::Mat uimat)
{
	int screenrotamount = Config::screenRotation();

	if (screenrotamount == 0)
	{
		return uimat;
	}

	//Mat size will always be the output res, which doesn't itself ever rotate. 
	sl::Resolution zedres = zed->getCameraInformation().camera_configuration.resolution;
	cv::Mat rotatedmat(cv::Size(zedres.width, zedres.height), CV_8UC4);

	cv::Point2d centerpoint;

	if (screenrotamount == 1)
	{
		centerpoint = cv::Point2d(zedres.width / 2.0, zedres.width / 2.0);
	}
	else if (screenrotamount == 2)
	{
		centerpoint = cv::Point2d(zedres.width / 2.0, zedres.height / 2.0);
	}
	else if (screenrotamount == 3)
	{
		centerpoint = cv::Point2d(zedres.height / 2.0, zedres.height / 2.0);
	}

	float rotangle = screenrotamount * 90.0;
	cv::Mat screenrotmatrix = cv::getRotationMatrix2D(centerpoint, -rotangle, 1); //No need for scale, as the UI was built with res in mind. 
	cv::warpAffine(uimat, rotatedmat, screenrotmatrix, rotatedmat.size());

	return rotatedmat;
}


///Converts a screen space point relative to the ZED image to where it should be placed in the UI
///to appear in the same place once the camera and UI rotations are finished. 
cv::Point ImageHelper::RawImagePointToRotated(cv::Point inpoint, bool scale )
{
	int imagerotamount = Config::imageRotation();

	sl::Resolution zedres = zed->getCameraInformation().camera_configuration.resolution;
	float zedwidth = zedres.width; //Shorthand.
	float zedheight = zedres.height; //Shorthand. 

	//First we determine where that point will be relative to the screen once the camera image is rotated/scaled. 
	//First rotation, then scale. 
	cv::Point rotscreenpoint;
	if (imagerotamount == 0)
	{
		rotscreenpoint = inpoint;
	}
	else
	{

		float halfwidth = zedwidth / 2; //Shorthand.
		float halfheight = zedheight / 2; //Shorthand.

		float xdistfromcenter = -halfwidth + inpoint.x;
		float ydistfromcenter = -halfheight + inpoint.y;

		//Scale. 
		float xscaleddist;
		float yscaleddist;
		float scalefactor = GetRotationScale();

		if (scale == false || scalefactor == 1.0)
		{
			xscaleddist = xdistfromcenter;
			yscaleddist = ydistfromcenter;
		}
		else
		{
			xscaleddist = xdistfromcenter * scalefactor;
			yscaleddist = ydistfromcenter * scalefactor;
		}

		//Gotta test all of these. 
		if (imagerotamount == 1)
		{
			//rotscreenpoint.x = inpoint.y;
			//rotscreenpoint.y = inpoint.x;
			rotscreenpoint.x = halfwidth - yscaleddist;
			rotscreenpoint.y = halfheight + xscaleddist;
		}
		else if (imagerotamount == 2)
		{
			//rotscreenpoint.x = zedwidth - inpoint.x;
			//rotscreenpoint.y = zedheight - inpoint.y;
			rotscreenpoint.x = halfwidth - xscaleddist;
			rotscreenpoint.y = halfheight - yscaleddist;
		}
		else if (imagerotamount == 3)
		{
			rotscreenpoint.x = halfwidth + yscaleddist;
			rotscreenpoint.y = halfheight - xscaleddist;
			//rotscreenpoint.x = zedheight - inpoint.y;
			//rotscreenpoint.y = zedwidth - inpoint.x;
		}
		else
		{
			cout << "Tried to rotate image based on invalid amount " << imagerotamount << ". Should be 0-3." << endl;
			return inpoint;
		}
	}

	//Now rotate to account for the final UI position. 
	int screenrotamount = Config::screenRotation();

	if (screenrotamount == 0)
	{
		return rotscreenpoint;
	}
	else
	{
		cv::Point rotuipoint;

		if (screenrotamount == 1)
		{
			rotuipoint.x = rotscreenpoint.y;
			rotuipoint.y = zedwidth - rotscreenpoint.x;
		}
		else if (screenrotamount == 2)
		{
			rotuipoint.x = zedwidth - rotscreenpoint.x;
			rotuipoint.y = zedheight - rotscreenpoint.y;
		}
		else if (screenrotamount == 3)
		{
			rotuipoint.x = zedheight - rotscreenpoint.y;
			rotuipoint.y = rotscreenpoint.x;
		}
		else
		{
			cout << "Tried to rotate ui based on invalid amount " << screenrotamount << ". Should be 0-3." << endl;
			return inpoint;
		}

		return rotuipoint;
	}

}

//int2 ImageHelper::ScreenTouchToUIPoint(int* x, int* y)
void ImageHelper::ScreenTouchToUIPoint(int* x, int* y)
{
	int screenrotamount = Config::screenRotation();
	int2 returnint;
	if (screenrotamount == 0)
	{
		/*
		returnint.x = x;
		returnint.y = y;
		return returnint;
		*/
	}
	else
	{
		sl::Resolution zedres = zed->getCameraInformation().camera_configuration.resolution;
		float zedwidth = zedres.width; //Shorthand.
		float zedheight = zedres.height; //Shorthand.

		if (screenrotamount == 1)
		{
			int oldx = *x;
			*x = *y;
			*y = (zedwidth - oldx);
			//returnint.y = zedwidth - x;
		}
		else if (screenrotamount == 2)
		{
			*x = (zedwidth - *x);
			*y = (zedheight - *y);
			//returnint.x = zedwidth - x;
			//returnint.y = zedheight - y;
		}
		else if (screenrotamount == 3)
		{
			int oldx = *x;
			*x = (zedheight - *y);
			*y = oldx;
			//returnint.x = zedheight - y;
			//returnint.y = x;
		}
	}
}