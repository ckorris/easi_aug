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
		//centerpoint = cv::Point2d(zedres.width / 2, zedres.height / 2);
	}
	else if (imagerotamount == 2) //Same dimensions as input since we just flipped it upside down. 
	{
		rotatedmat = cv::Mat(cv::Size(zedres.width, zedres.height), CV_8UC4);
		//centerpoint = cv::Point2d(zedres.width / 2, zedres.height / 2);
	}
	else if (imagerotamount == 3)
	{
		rotatedmat = cv::Mat(cv::Size(zedres.height, zedres.width), CV_8UC4); //Currently duplicate of == 1. 
		//centerpoint = cv::Point2d(zedres.width / 2, zedres.height / 2);
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

///Converts a screen space point relative to the ZED image to where it should be placed in the UI
///to appear in the same place once the camera and UI rotations are finished. 
cv::Point ImageHelper::RawImagePointToRotated(cv::Point inpoint)
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
		if (scalefactor == 1.0)
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
			cout << "Tried to rotate image based on invalid height " << imagerotamount << ". Should be 0-3." << endl;
			return inpoint;
		}

		
	}

	//For now just return that. Gotta validate the above code before moving to UI. 
	return rotscreenpoint;

}