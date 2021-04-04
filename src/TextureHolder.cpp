#include <TextureHolder.h>
#include <CamUtilities.h>

TextureHolder::TextureHolder(sl::Mat *zedImage, sl::Mat *depthMeasure, cv::Mat *ocvImage)
{
	_zedImage = zedImage;
	_depthMeasure = depthMeasure;
	_ocvImage = ocvImage;

	_isSetUp = false;

}


void TextureHolder::CreateMatrixesFromZed(sl::Camera *zed)
{
	if (_isSetUp == true)
	{
		_zedImage->free();
		_depthMeasure->free();
		_ocvImage->release();
	}

	sl::CameraInformation cameraInfo = zed->getCameraInformation();
	sl::Resolution size = cameraInfo.camera_configuration.resolution;

	Mat zedimage = sl::Mat(size, sl::MAT_TYPE::U8_C4);

	zedimage.move(*_zedImage);

	Mat depthmeasure = sl::Mat(size, sl::MAT_TYPE::F32_C1);
	depthmeasure.move(*_depthMeasure);

	int cvmattype = CamUtilities::slMatType2cvMatType(_zedImage->getDataType());

	_ocvImage->create(size.height, size.width, cvmattype);
	_ocvImage->data = _zedImage->getPtr<sl::uchar1>(sl::MEM::CPU);

	
	_isSetUp = true;
}
