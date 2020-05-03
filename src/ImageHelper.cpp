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
	return 1.0;

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
		/*
		//cv::Mat_<float> pointmat(rotscreenpoint, false);
		cv::Mat_<float> pointmat(2, 1);
		pointmat.at<float>(0, 0) = (float)inpoint.x;
		//pointmat.at<float>(0, 0) = 43.2;
		pointmat(1, 0) = (float)inpoint.y;
		//pointmat(2, 0) = 1.0;
		//pointmat(0, 0) = rotscreenpoint.x;
		cout << "Start: " <<  pointmat.at<float>(0, 0) << ", " << pointmat.at<float>(0, 1) << endl;

		float rotangle = imagerotamount * 90.0;

		cv::Point2d centerpoint = cv::Point2d(zedwidth / 2, zedheight / 2);
		//cv::Mat imagerotmatrix = cv::getRotationMatrix2D(centerpoint, -rotangle, GetRotationScale());
		cv::Mat imagerotmatrix = cv::getRotationMatrix2D(centerpoint, -rotangle, 1);

		cv::Mat_<float> outpoint(2, 1);
		//cv::warpAffine(pointmat, outpoint, imagerotmatrix, pointmat.size());
		//cv::transform(pointmat, outpoint, imagerotmatrix);

		//cout << "End: " << outpoint.at<float>(0, 0) << ", " << outpoint.at<float>(0, 1) << endl;
		*/
		
		float halfwidth = zedwidth / 2; //Shorthand.
		float halfheight = zedheight / 2; //Shorthand.

		float xdistfromcenter = -halfwidth + inpoint.x;
		float ydistfromcenter = -halfheight + inpoint.y;

		//Gotta test all of these. 
		if (imagerotamount == 1)
		{
			//rotscreenpoint.x = inpoint.y;
			//rotscreenpoint.y = inpoint.x;
			rotscreenpoint.x = halfwidth - ydistfromcenter;
			rotscreenpoint.y = halfheight + xdistfromcenter;
		}
		else if (imagerotamount == 2)
		{
			//rotscreenpoint.x = zedwidth - inpoint.x;
			//rotscreenpoint.y = zedheight - inpoint.y;
			rotscreenpoint.x = halfwidth - xdistfromcenter;
			rotscreenpoint.y = halfheight - ydistfromcenter;
		}
		else if (imagerotamount == 3)
		{
			rotscreenpoint.x = halfwidth + ydistfromcenter;
			rotscreenpoint.y = halfheight - xdistfromcenter;
			//rotscreenpoint.x = zedheight - inpoint.y;
			//rotscreenpoint.y = zedwidth - inpoint.x;
		}
		else
		{
			cout << "Tried to rotate image based on invalid height " << imagerotamount << ". Should be 0-3." << endl;
			return inpoint;
		}

		//Now scale. 
		float scalefactor = GetRotationScale();

		
		//if (scalefactor != 1) //No need to do maths if we're not scaling it. This would happen if flipping exactly upside down.
		if(false) //TEST
		{
			//scalefactor = 1.0; //TEST

			cout << "Input: " << inpoint << endl;

			float newimagewidth = (imagerotamount % 2 == 0) ? zedwidth : zedheight;
			float newimageheight = (imagerotamount % 2 == 0) ? zedheight : zedwidth;

			cout << "New Width: " << newimagewidth << " New Height: " << newimageheight << endl;

			//All this makes sense to me but there's a lot of places for things to go wrong. 
			float halfwidth = newimagewidth / 2; //Shorthand.
			float halfheight = newimageheight / 2; //Shorthand.

			cout << "Half width: " << halfwidth << " Half height: " << halfheight << endl;

			cout << "Rotated: " << rotscreenpoint << endl;

			float xdistfromcenter = -halfwidth + rotscreenpoint.x;
			float ydistfromcenter = -halfheight + rotscreenpoint.y;

			cout << "X Center dist: " << xdistfromcenter << " Y Center Dist: " << ydistfromcenter << endl;

			float xfactorapplied = abs(xdistfromcenter) / halfwidth;
			float yfactorapplied = abs(ydistfromcenter) / halfheight;

			rotscreenpoint.x = halfwidth + xdistfromcenter * (scalefactor * xfactorapplied);
			rotscreenpoint.y = halfheight + ydistfromcenter * (scalefactor * yfactorapplied);

			cout << "Out: " << rotscreenpoint << endl;
			cout << endl;
		}
	}

	//For now just return that. Gotta validate the above code before moving to UI. 
	return rotscreenpoint;

}