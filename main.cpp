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
// Sketching
//=======================================================================================

void sketchingGray(const Mat & grayImg)
{
  	Mat gradX, gradY;
  	Mat white, overlay;

	Sobel(grayImg, gradX, -1, 0, 1);
    Sobel(grayImg, gradY, -1, 1, 0);

    createWhiteImage(grayImg.rows,grayImg.cols,white);

	white.copyTo(overlay);

    int count = 20000;
    double alpha = 0.5;
    int length = 5;
    for (int i = 0; i < count; ++i)
    {
    	int u = rand() % grayImg.rows;
    	int v = rand() % grayImg.cols;

    	int valGradX = gradX.at<unsigned char>(u,v);
    	int valGradY = gradY.at<unsigned char>(u,v);

    	float norm = sqrt(valGradY*valGradY + valGradX* valGradX);

    	//cout << norm << endl;

    	if(norm > 40)
    	{
    		white.copyTo(overlay);
			line(overlay, Point(v-((valGradX/norm)*length),u-((-valGradY/norm)*length)), Point(v+((valGradX/norm)*length),u+((-valGradY/norm)*length)), Scalar(0,0,0), 1);
			addWeighted(overlay, alpha, white, 1 - alpha, 0, white);
		}
		else
		{
			i--;
		}
    }

	imshow("white", white);
	imshow("gradX", gradX);
	imshow("gradY", gradY);
	//imshow("gradY", norm_0_255(gradY));
	cvWaitKey();
}

void sketching(const Mat & img)
{
  	Mat gradX, gradY;
  	Mat gray;
  	Mat white, overlay;

  	toGray(img,gray);
	Sobel(gray, gradX, -1, 0, 1);
    Sobel(gray, gradY, -1, 1, 0);

    createWhiteImage(img.rows,img.cols,white);

	//Mat visited = zeros(img.rows,img.cols,CV_8U);
	Mat visited = Mat(img.rows,img.cols, CV_8UC1, Scalar(0));

    int count = 5000;
    int length = 10;
    int thickness = 1;
    float percent = 0.2;
    bool BW = true;
    for (int i = 0; i < count; ++i)
    {
    	int u = rand() % img.rows;
    	int v = rand() % img.cols;

    	int valGradX = gradX.at<unsigned char>(u,v);
    	int valGradY = gradY.at<unsigned char>(u,v);

    	float norm = sqrt(valGradY*valGradY + valGradX* valGradX);

    	if(norm > 40)
    	{
    		LineIterator lineIterator(white, Point(v-((valGradX/norm)*length),u-((-valGradY/norm)*length)), Point(v+((valGradX/norm)*length),u+((-valGradY/norm)*length)), 8, true);
    		for (int i = 0; i < lineIterator.count; ++i, ++lineIterator)
    		{
    			for(int x = -thickness; x <= thickness; x++)
		        {
		            for(int  y= -thickness; y <= thickness; y++)
		            {
		                Point pos = lineIterator.pos() + Point(x,y);
		                
		                if(pos.x >= 0 && pos.x < img.cols && pos.y >= 0 && pos.y < img.rows)
                		{
                    		if(visited.at<unsigned char>(pos.x, pos.y) == 0)
                    		{
                    			Vec3b& pix = white.at<Vec3b>(pos);
                    			const Vec3b& temp = BW ? Vec3b(255, 255, 255) : img.at<Vec3b>(pos);

                    			pix -= temp * percent;

                    			visited.at<unsigned char>(pos) = (unsigned char)1;
                    		}
                    	}
		    		}
		    	}
		    }

		}
		else
		{
			i--;
		}
    }

	imshow("white", white);
	imshow("gradX", gradX);
	imshow("gradY", gradY);

	cvWaitKey();
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
  	Mat white, overlay;


  	// Ouvrir l'image d'entr�e et v�rifier que l'ouverture du fichier se d�roule normalement
  	inputImageSrc = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  	if(!inputImageSrc.data ) { // Check for invalid input
  		std::cout <<  "Could not open or find the image " << argv[1] << std::endl ;
		waitKey(0); // Wait for a keystroke in the window
        return -1;
    }

    toGray(inputImageSrc, inputImageSrcGray);

    //sketchingGray(inputImageSrcGray);
    sketching(inputImageSrc);

  	return 0;
}
