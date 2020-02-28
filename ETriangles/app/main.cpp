#include "ETriangleWidget.h"
#include <QtWidgets/QApplication>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <QString>

#pragma comment(lib, "opencv_core330d.lib") 
#pragma comment(lib, "opencv_objdetect330d.lib")
#pragma comment(lib, "opencv_video330d.lib") 
#pragma comment(lib, "opencv_photo330d.lib") 
#pragma comment(lib, "opencv_stitching330d.lib") 
#pragma comment(lib, "opencv_superres330d.lib") 
#pragma comment(lib, "opencv_videostab330d.lib") 
#pragma comment(lib, "opencv_calib3d330d.lib") 
#pragma comment(lib, "opencv_features2d330d.lib") 
#pragma comment(lib, "opencv_flann330d.lib") 
#pragma comment(lib, "opencv_highgui330d.lib") 
#pragma comment(lib, "opencv_imgproc330d.lib") 
#pragma comment(lib, "opencv_ml330d.lib")
#pragma comment(lib, "opencv_videoio330d.lib")
#pragma comment(lib, "opencv_shape330d.lib")
#pragma comment(lib, "opencv_imgcodecs330d.lib")
#pragma comment(lib, "opencv_dnn330d.lib")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ETriangleWidget w;
	w.setMinimumWidth(800);
	w.setMinimumHeight(600);
	w.show();
	return a.exec();
}
