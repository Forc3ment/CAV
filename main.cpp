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
// Sketching Lines
//=======================================================================================

// void sketchingGray(const Mat & grayImg)
// {
//   	Mat gradX, gradY;
//   	Mat white, overlay;

// 	Sobel(grayImg, gradX, -1, 0, 1);
//     Sobel(grayImg, gradY, -1, 1, 0);

//     createWhiteImage(grayImg.rows,grayImg.cols,white);

// 	white.copyTo(overlay);

//     int count = 20000;
//     double alpha = 0.5;
//     int length = 5;
//     for (int i = 0; i < count; ++i)
//     {
//     	int u = rand() % grayImg.rows;
//     	int v = rand() % grayImg.cols;

//     	int valGradX = gradX.at<unsigned char>(u,v);
//     	int valGradY = gradY.at<unsigned char>(u,v);

//     	float norm = sqrt(valGradY*valGradY + valGradX* valGradX);

//     	//cout << norm << endl;

//     	if(norm > 40)
//     	{
//     		white.copyTo(overlay);
// 			line(overlay, Point(v-((valGradX/norm)*length),u-((-valGradY/norm)*length)), Point(v+((valGradX/norm)*length),u+((-valGradY/norm)*length)), Scalar(0,0,0), 1);
// 			addWeighted(overlay, alpha, white, 1 - alpha, 0, white);
// 		}
// 		else
// 		{
// 			i--;
// 		}
//     }

// 	imshow("white", white);
// 	imshow("gradX", gradX);
// 	imshow("gradY", gradY);
// 	//imshow("gradY", norm_0_255(gradY));
// 	cvWaitKey();
// }

void sketchingLines(const Mat & img)
{
	cout << "------ Sketching Lines ------" << endl;
	Mat gradX, gradY;
  	Mat gray, gray16;
  	Mat white, overlay;

  	toGray(img,gray);

  	Sobel(gray, gradX, CV_16S, 0, 1, 3);
    Sobel(gray, gradY, CV_16S, 1, 0, 3);

    gradX = abs(gradX);
    gradY = abs(gradY);

    gradX.convertTo(gradX, CV_8U);
    gradY.convertTo(gradY, CV_8U);

    createWhiteImage(img.rows,img.cols,white);

    int count = 5000;
    int length = 10;
    int thickness = 0;
    float percent = 0.2;
    bool BW = true;
    for (int i = 0; i < count; ++i)
    {
    	Mat visited = Mat(img.rows,img.cols, CV_8UC1, Scalar(0));
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
                    		if(visited.at<unsigned char>(pos) == 0)
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

	imwrite("sketchingLinesGradient.jpg", white);
	cout << "------ Saving Sketching ------" << endl;
}

//=======================================================================================
// Sketching Spline
//=======================================================================================

typedef vector<double> vec;

struct SplineSet{
    double a;
    double b;
    double c;
    double d;
    double x;
};

vector<SplineSet> spline(vec &x, vec &y)
{
    int n = x.size()-1;
    vec a;
    a.insert(a.begin(), y.begin(), y.end());
    vec b(n);
    vec d(n);
    vec h;

    for(int i = 0; i < n; ++i)
        h.push_back(x[i+1]-x[i]);

    vec alpha;
    for(int i = 0; i < n; ++i)
        alpha.push_back( 3*(a[i+1]-a[i])/h[i] - 3*(a[i]-a[i-1])/h[i-1]  );

    vec c(n+1);
    vec l(n+1);
    vec mu(n+1);
    vec z(n+1);
    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;

    for(int i = 1; i < n; ++i)
    {
        l[i] = 2 *(x[i+1]-x[i-1])-h[i-1]*mu[i-1];
        mu[i] = h[i]/l[i];
        z[i] = (alpha[i]-h[i-1]*z[i-1])/l[i];
    }

    l[n] = 1;
    z[n] = 0;
    c[n] = 0;

    for(int j = n-1; j >= 0; --j)
    {
        c[j] = z [j] - mu[j] * c[j+1];
        b[j] = (a[j+1]-a[j])/h[j]-h[j]*(c[j+1]+2*c[j])/3;
        d[j] = (c[j+1]-c[j])/3/h[j];
    }

    vector<SplineSet> output_set(n);
    for(int i = 0; i < n; ++i)
    {
        output_set[i].a = a[i];
        output_set[i].b = b[i];
        output_set[i].c = c[i];
        output_set[i].d = d[i];
        output_set[i].x = x[i];
    }
    return output_set;
}

void sketchingSplines(const Mat & img)
{

	cout << "------ Sketching Splines ------" << endl;

	Mat gradX, gradY;
  	Mat gray, gray16;
  	Mat white, overlay;

  	toGray(img,gray);

  	Sobel(gray, gradX, CV_16S, 0, 1, 3);
    Sobel(gray, gradY, CV_16S, 1, 0, 3);

    gradX = abs(gradX);
    gradY = abs(gradY);

    gradX.convertTo(gradX, CV_8U);
    gradY.convertTo(gradY, CV_8U);

    createWhiteImage(img.rows,img.cols,white);

    int count = 10000;
    int length = 10;
    int thickness = 0;
    float percent = 0.05;
    bool BW = true;
    int maxSize = 20;
    int threshold = 150;

    for (int i = 0; i < count; ++i)
    {
    	Mat visited = Mat(img.rows,img.cols, CV_8UC1, Scalar(0));
    	vec X, Y;

    	int u = rand() % img.rows;
    	int v = rand() % img.cols;

    	int valGradX = gradX.at<unsigned char>(u,v);
    	int valGradY = gradY.at<unsigned char>(u,v);

    	float norm = sqrt(valGradY*valGradY + valGradX* valGradX);

    	if(norm > threshold)
    	{
    		X.push_back(v);
    		Y.push_back(u);
			
    		for (int i = 0; i < maxSize; ++i)
    		{
    			u+=(-valGradY/norm)*length;
    			v+=(valGradX/norm)*length;

    			if(v >= 0 && v < img.cols && u >= 0 && u < img.rows)
    			{
    				valGradX = gradX.at<unsigned char>(u, v);
    				valGradY = gradY.at<unsigned char>(u, v);

		   			norm = sqrt(valGradY*valGradY + valGradX*valGradX);
	    			if(norm > threshold)
	    			{
	    				X.push_back(v);
	    				Y.push_back(u);
	    			}
	    			else break;
    			}
    			else break;
    		}

    		if(X.size() == 1)
    		{
    			
    		}
    		else
    		{
	    		vector<SplineSet> cs = spline(X, Y);
			    for(unsigned int i = 0; i < cs.size(); ++i)
			    { 
			    	double tempX1, tempX2;
			    	if(X[i] < X[i+1]) 
			    	{
			    		tempX1 = X[i];
			    		tempX2 = X[i+1];
			    	}
			    	else
			    	{
			    		tempX2 = X[i];
			    		tempX1 = X[i+1];
			    	}

			    	const Vec3b& temp = BW ? Vec3b(255, 255, 255) : img.at<Vec3b>(Point(X[0], Y[0]));

			        for (double t = tempX1; t < tempX2; t+=0.05)
			        {
			        	for(int x = -thickness; x <= thickness; x++)
					    {
					        for(int  y= -thickness; y <= thickness; y++)
					        {
					           	double St = cs[i].a + cs[i].b * (t-X[i]) + cs[i].c * (t-X[i]) * (t-X[i]) + cs[i].d * (t-X[i]) * (t-X[i]) * (t-X[i]);
					            Point pos = Point((int)t,(int)St) + Point(x,y);

					            if(pos.x >= 0 && pos.x < img.cols && pos.y >= 0 && pos.y < img.rows)
			               		{
			                  		if(visited.at<unsigned char>(pos) == 0)
			                   		{
			                   			Vec3b& pix = white.at<Vec3b>(pos);
			                   			
		                    			if(BW) pix -= temp * percent;
		                    			else pix = temp;

		                    			visited.at<unsigned char>(pos) = (unsigned char)1;
		                    		}
		                    	}
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

	imwrite("sketchingSplinesGradient.jpg", white);
	// imshow("white", white);
	// imshow("GradX", gradX);
	// imshow("GradY", gradY);
	cout << "------ Saving Sketching ------" << endl;
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

  	// Ouvrir l'image d'entr�e et v�rifier que l'ouverture du fichier se d�roule normalement
  	inputImageSrc = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  	if(!inputImageSrc.data ) { // Check for invalid input
  		std::cout <<  "Could not open or find the image " << argv[1] << std::endl ;
		waitKey(0); // Wait for a keystroke in the window
        return -1;
    }

    sketchingSplines(inputImageSrc);

    sketchingLines(inputImageSrc);

	cvWaitKey();
  	return 0;
}
