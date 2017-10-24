#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>

using namespace cv;
using namespace std;

string toString(int i) // convert int to string
{
    std::stringstream value;
    value << i;
    return value.str();
}

//==================================================================================  a =====
// Mat norm_0_255(InputArray _src)
// Create and return normalized image
//=======================================================================================
Mat norm_0_255(InputArray _src) {
 Mat src = _src.getMat();
 // Create and return normalized image:
 Mat dst;
 switch(src.channels()) {
	case 1:
		normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
	src.copyTo(dst);
	break;
 }
 return dst;
}

//=======================================================================================
// to YCrCb
//=======================================================================================
void toYCrCb(const Mat & imgSrc, Mat &ImgSrcYCrCb){
	cvtColor(imgSrc, ImgSrcYCrCb, CV_BGR2YCrCb);
}

//=======================================================================================
// to Lab
//=======================================================================================
void toLab(const Mat & imgSrc, Mat &ImgSrcLab){
	cvtColor(imgSrc, ImgSrcLab, CV_BGR2Lab);
}

//=======================================================================================
// to Gray
//=======================================================================================
void toGray(const Mat & imgSrc, Mat &ImgSrcLab){
	cvtColor(imgSrc, ImgSrcLab, CV_BGR2GRAY);
}

void createWhiteImage(int u, int v, Mat & white){
	white = Mat(u, v, CV_8UC3, Scalar(255,255,255));
}



//=======================================================================================
//=======================================================================================
// MAIN
//=======================================================================================
//=======================================================================================
int main(int argc, char** argv){
	if (argc < 2){
    	std::cout << "No image data or not enough ... At least one argument is required! \n";
    	return -1;
  	}

  	Mat inputImageSrc;
  	Mat inputImageSrcGray;
  	Mat gradX, gradY;
  	Mat white;


  	// Ouvrir l'image d'entr�e et v�rifier que l'ouverture du fichier se d�roule normalement
  	inputImageSrc = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  	if(!inputImageSrc.data ) { // Check for invalid input
  		std::cout <<  "Could not open or find the image " << argv[1] << std::endl ;
		waitKey(0); // Wait for a keystroke in the window
        return -1;
    }

    toGray(inputImageSrc, inputImageSrcGray);

    Sobel(inputImageSrcGray, gradX, -1, 0, 1);
    Sobel(inputImageSrcGray, gradY, -1, 1, 0);

    createWhiteImage(inputImageSrc.rows,inputImageSrc.cols,white);

    int count = 500000;
    for (int i = 0; i < count; ++i)
    {
    	int u = rand() % inputImageSrc.rows;
    	int v = rand() % inputImageSrc.cols;

    	int valGradX = gradX.at<unsigned char>(u,v);
    	int valGradY = gradY.at<unsigned char>(u,v);

    	float norm = sqrt(valGradY*valGradY + valGradX* valGradX);

    	if(norm > 0.2){
			line(white, Point(v,u), Point(v+(valGradY/norm),u+(-valGradX/norm)), Scalar(0,0,0) );
		}
    }

	imshow("white", white);
	imshow("gradX", gradX);
	imshow("gradY", gradY);
	//imshow("gradY", norm_0_255(gradY));
	cvWaitKey();

  	return 0;
}
