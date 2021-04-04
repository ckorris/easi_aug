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
		//zedImage.free();
		//depth_measure.free();
		//image_ocv.release();
	}

	sl::CameraInformation cameraInfo = zed->getCameraInformation();
	sl::Resolution size = cameraInfo.camera_configuration.resolution;

	Mat zedimage = sl::Mat(size, sl::MAT_TYPE::U8_C4);
	//zedimage.copyTo(*_zedImage, sl::COPY_TYPE::CPU_CPU);
	//_zedImage->free(sl::MEM::CPU);
	//_zedImage->alloc(size, sl::MAT_TYPE::U8_C4);
	zedimage.move(*_zedImage);
	//_zedImage->setFrom(zedimage, sl::COPY_TYPE::CPU_CPU);

	Mat depthmeasure = sl::Mat(size, sl::MAT_TYPE::F32_C1);
	//depthmeasure.copyTo(*_depthMeasure, sl::COPY_TYPE::CPU_CPU);
	depthmeasure.move(*_depthMeasure);

	int cvmattype = CamUtilities::slMatType2cvMatType(_zedImage->getDataType());
	cv::Mat image_ocv(size.height, size.width, cvmattype, _zedImage->getPtr<sl::uchar1>(sl:: MEM::CPU));

	image_ocv.copyTo(*_ocvImage);
	
	_isSetUp = true;
}
