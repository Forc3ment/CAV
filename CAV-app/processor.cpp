#include "processor.h"

using namespace cv;
using namespace std;

Processor::Processor()
{

}

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
void toGray(const Mat & imgSrc, Mat &ImgSrcGray){
    cvtColor(imgSrc, ImgSrcGray, CV_BGR2GRAY);
}

//=======================================================================================
// to YUV
//=======================================================================================
void toYUV(const Mat & imgSrc, Mat &ImgSrcYUV){
    cvtColor(imgSrc, ImgSrcYUV, CV_BGR2YUV);
}


//=======================================================================================
// Gradient
//=======================================================================================
void Processor::getGradient(const Mat & img, Mat &gradX, Mat &gradY){

    Mat gray, imgBlur;

    toGray(img,gray);

    GaussianBlur(gray, imgBlur, Size(3,3),0);

    Sobel(imgBlur, gradX, CV_16S, 0, 1, 3);
    Sobel(imgBlur, gradY, CV_16S, 1, 0, 3);

    gradX = abs(gradX);
    gradY = abs(gradY);

    gradX.convertTo(gradX, CV_8U);
    gradY.convertTo(gradY, CV_8U);
}

//=======================================================================================
// Tensor
//=======================================================================================

void Processor::getSingularValue(const float a, const float b, const float d, Vec2f &singularVector, Vec2f &singularValue)
{
    Mat A(2,2, CV_32F);
    A.at<float>(0,0) = a;
    A.at<float>(0,1) = A.at<float>(1,0) = b;
    A.at<float>(1,1) = d;

    Mat w, u, vt; //w -> singular values, u -> left singular vectors, vt -> right singular vectors transposed

    SVD::compute(A, w, u, vt); // Calcul de la décomposition en valeurs/vecteurs propres

    singularVector[0] = u.at<float>(0,0);
    singularVector[1] = u.at<float>(0,1);

    singularValue = w;
}

void Processor::getTensor(const Mat & img, Mat &a, Mat &b, Mat &d, Mat &tensor)
{
    Mat imgBlur;
    Mat gX, gY;
    Mat white = Mat(img.size(), CV_32F);
    Vec2f singularVector, singularValue;

    vector<Mat> gradX, gradY, YUV;

    a = Mat(img.size(), CV_32F);
    b = Mat(img.size(), CV_32F);
    d = Mat(img.size(), CV_32F);

    //GaussianBlur(img, imgBlur, Size(7,7),0);
    bilateralFilter(img, imgBlur,6, 150, 150);

    imshow("blur", imgBlur);

    Sobel(imgBlur, gX, CV_32F, 0, 1, 5);
    Sobel(imgBlur, gY, CV_32F, 1, 0, 5);

    split(gX,gradX);
    split(gY,gradY);

    for (int i = 0; i < img.rows; ++i)
    {
        for (int j = 0; j < img.cols; ++j)
        {
            // Recuperation des gradiens des 3 composantes.
            float gXB = gradX[0].at<float>(i,j);
            float gYB = gradY[0].at<float>(i,j);

            float gXG = gradX[1].at<float>(i,j);
            float gYG = gradY[1].at<float>(i,j);

            float gXR = gradX[2].at<float>(i,j);
            float gYR = gradY[2].at<float>(i,j);

            //Calcul des 4 valeurs du tenseur en chaques points de l'image (a,b;c,d) avec b=c car matrice symétrique
            a.at<float>(i,j) = gXB * gXB + gXG * gXG + gXR * gXR;
            b.at<float>(i,j) = gXB * gYB + gXG * gYG + gXR * gYR;
            d.at<float>(i,j) = gYB * gYB + gYG * gYG + gYR * gYR;

            // Stockage dans une image pour simplifier l'acces a la valeur du tenseur (et la ramener ensuite entre 0 et 255)
            getSingularValue(a.at<float>(i,j), b.at<float>(i,j), d.at<float>(i,j), singularVector, singularValue);
            white.at<float>(i, j) = abs(singularValue[0] - singularValue[1]);
        }
    }

    tensor = norm_0_255(white);
    //imshow("tensor", tensor);
    imwrite("tensor.jpg", tensor);
}

void Processor::drawTensor(const Mat & img)
{
    Mat a, b, d;
    Vec2f singularVector, singularValue;
    Mat white = Mat(img.size(), CV_32F);

    getTensor(img, a, b, d, white);

    imshow("white", norm_0_255(white));

    cvWaitKey();
}

void Processor::createWhiteImage(int u, int v, Mat & white)
{
    white = Mat(u, v, CV_8UC3, Scalar(255,255,255));
}

//=======================================================================================
// Sketching Lines with Tensor
//=======================================================================================

Mat Processor::sketchingLinesWithTensor(const Mat & img)
{
    cout << "------ Sketching Lines with Tensor ------" << endl;
    Mat a, b, d;
    Vec2f singularVector, singularValue;
    Mat white;
    Mat tensor;

    //Calcul du tenseur et des matrices a b et d
    getTensor(img, a, b, d, tensor);

    createWhiteImage(img.rows,img.cols,white);

    int count = m_count;
    int length = m_length;
    int thickness = m_thickness;
    float percent = m_percent;
    int threshold = m_threshold;
    bool BW = !m_clicked;

    for (int i = 0; i < count; ++i)
    {
        // image pour eviter de colorier la meme case sur un passage
        Mat visited = Mat(img.rows,img.cols, CV_8UC1, Scalar(0));

        // Récupération d'un point aléatoire de l'image
        int u = rand() % img.rows;
        int v = rand() % img.cols;

        //Et des valeurs des matrices a, b et d pour calculer les valeurs et vecteurs propres
        float valA = a.at<float>(u,v);
        float valB = b.at<float>(u,v);
        float valD = d.at<float>(u,v);

        getSingularValue(valA, valB, valD, singularVector, singularValue);

        // récupération de la valeurs du tenseur qui a ete reprojeté entre 0 et 255
        float norm = tensor.at<unsigned char>(u,v);

        if(norm > threshold)
        {
            //Choix de deux point autour de celui tiré aléatoiremennt en fonction des vecteurs propres calculé précédemment
            //On tracera une ligne entre ces deux points.
            LineIterator lineIterator(white,
                    Point( v-(singularVector[0]*length) ,u-(singularVector[1]*length)),
                    Point( v+(singularVector[0]*length) ,u+(singularVector[1]*length)),
                    8, true);

            for (int i = 0; i < lineIterator.count; ++i, ++lineIterator)
            {
                //boucle x et y pour gérer l'épaisseur de la ligne
                for(int x = -thickness; x <= thickness; x++)
                {
                    for(int  y= -thickness; y <= thickness; y++)
                    {
                        // récupération de la position grace à l'itérateur et modification selon l'épaisseur
                        Point pos = lineIterator.pos() + Point(x,y);

                        //vérification si on sort pas de l'image
                        if(pos.x >= 0 && pos.x < img.cols && pos.y >= 0 && pos.y < img.rows)
                        {
                            if(visited.at<unsigned char>(pos) == 0)
                            {
                                // recuperation de la couleur courante
                                Vec3b& pix = white.at<Vec3b>(pos);

                                //calcul de la valeur qui sera utilisé pour colorier l'image
                                const Vec3b& temp = BW ? Vec3b(255, 255, 255) : img.at<Vec3b>(pos);

                                if(BW) pix -= temp * percent;
                                else pix = temp * percent;


                                visited.at<unsigned char>(pos) = (unsigned char)1;
                            }
                        }
                    }
                }
            }

        }
        else
        {
            // Si le seuil n'est pas dépassé alors on recommence l'itération.
            i--;
        }
    }


    imwrite("sketchingLinesTensor.jpg", white);
    cout << "------ Saving Sketching ------" << endl;
    imshow("white", white);
    return white;
}


//=======================================================================================
// Sketching Lines with Gradient
//=======================================================================================
Mat Processor::sketchingLinesWithGradient(const Mat & img)
{
    // Commentaires complet dans la fonction sketchingLinesWithTensor

    cout << "------ Sketching Lines with Gradient ------" << endl;
    Mat gradX, gradY;

    Mat white;

    //Calcul des gradient
    getGradient(img, gradX, gradY);

    createWhiteImage(img.rows,img.cols,white);

    int count = m_count;
    int length = m_length;
    int thickness = m_thickness;
    float percent = m_percent;
    int threshold = m_threshold;
    bool BW = !m_clicked;

    for (int i = 0; i < count; ++i)
    {
        Mat visited = Mat(img.rows,img.cols, CV_8UC1, Scalar(0));
        int u = rand() % img.rows;
        int v = rand() % img.cols;

        // Récupération des valeurs des grdients
        int valGradX = gradX.at<unsigned char>(u,v);
        int valGradY = gradY.at<unsigned char>(u,v);

        // Calcul de sa norme
        float norm = sqrt(valGradY*valGradY + valGradX* valGradX);

        if(norm > threshold)
        {
            //Ligne dans la direction de la normal au gradient
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

                                if(BW) pix -= temp * percent;
                                else pix = temp * percent;

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
    imshow("white", white);
    return white;
}

//=======================================================================================
// Spline
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

//=======================================================================================
// Sketching Splines with Tensor
//=======================================================================================

Mat Processor::sketchingSplinesWithTensor(const Mat & img)
{
    cout << "------ Sketching Splines with Tensor ------" << endl;
    Mat a, b, d;
    Vec2f singularVector, singularValue;
    Mat white;
    Mat tensor;

    getTensor(img, a, b, d, tensor);

    createWhiteImage(img.rows,img.cols,white);

    int count = m_count;
    int length = m_length;
    int thickness = m_thickness;
    float percent = m_percent;
    int threshold = m_threshold;
    bool BW = !m_clicked;
    int maxSize = m_maxSize;

    for (int i = 0; i < count; ++i)
    {
        vec X, Y;

        Mat visited = Mat(img.rows,img.cols, CV_8UC1, Scalar(0));

        int u = rand() % img.rows;
        int v = rand() % img.cols;

        float valA = a.at<float>(u,v);
        float valB = b.at<float>(u,v);
        float valD = d.at<float>(u,v);

        getSingularValue(valA, valB, valD, singularVector, singularValue);

        float norm = tensor.at<unsigned char>(u,v);

        if(norm > threshold)
        {
            //Sauvegardes du point aléatoire dans X et Y
            X.push_back(v);
            Y.push_back(u);

            for (int i = 0; i < maxSize; ++i)
            {
                //On regarde dans la direction de la normale au gradient
                u+=(singularVector[1]*length);
                v+=(singularVector[0]*length);

                //Si on est toujours dans l'image
                if(v >= 0 && v < img.cols && u >= 0 && u < img.rows)
                {
                    //Et si la valeur du tenseur est assez importante
                    float norm = tensor.at<unsigned char>(u,v);

                    if(norm > threshold)
                    {
                        //alors on sauve aussi le point et ainsi de suite jusqu'a un max de maxSize
                        X.push_back(v);
                        Y.push_back(u);
                    }
                    else break;
                }
                else break;
            }

            if(X.size() == 1)
            {
                i--; //Si X ne contient qu'un seul point on recommence l'itération
            }
            else
            {
                //Calcul des caractéristique des splines en fonction des point données
                vector<SplineSet> cs = spline(X, Y);

                for(unsigned int i = 0; i < cs.size(); ++i)
                {
                    // on tracera la splin de point avec le x le plus petit vers le x le plus grand
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

                    //Sauvegarde de la valeur du pixel au point initial.
                    const Vec3b& temp = BW ? Vec3b(255, 255, 255) : img.at<Vec3b>(Point(X[0], Y[0]));

                    //Parcours de la spline par de petits pas pour ne pas rater de point en Y
                    for (double t = tempX1; t < tempX2; t+=0.05)
                    {
                        for(int x = -thickness; x <= thickness; x++)
                        {
                            for(int  y= -thickness; y <= thickness; y++)
                            {
                                //Calcul du point a colorier
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


    imwrite("sketchingSplinesTensor.jpg", white);
    cout << "------ Saving Sketching ------" << endl;
    imshow("white", white);
    return white;
}

//=======================================================================================
// Sketching Spline with Gradient
//=======================================================================================

Mat Processor::sketchingSplinesWithGradient(const Mat & img)
{

    cout << "------ Sketching Splines with Gradient ------" << endl;

    Mat gradX, gradY;
    Mat white;

    getGradient(img, gradX, gradY);

    createWhiteImage(img.rows,img.cols,white);

    int count = m_count;
    int length = m_length;
    int thickness = m_thickness;
    float percent = m_percent;
    int threshold = m_threshold;
    bool BW = !m_clicked;
    int maxSize = m_maxSize;

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
    imshow("white", white);
    // imshow("GradX", gradX);
    // imshow("GradY", gradY);
    cout << "------ Saving Sketching ------" << endl;
    imshow("white", white);
    return white;
}


void Processor::process(Mat* img)
{
    switch ( m_algo ) {
    case 0:
      result = sketchingSplinesWithTensor(*img);
      break;

    case 1:
        cout << "------ Sketching Splines with Tensor ------" << endl;
      result = sketchingSplinesWithGradient(*img);
      break;

    case 2:
      result = sketchingLinesWithTensor(*img);
      break;

    case 3:
      result = sketchingLinesWithGradient(*img);
      break;

    default:
      result = sketchingSplinesWithTensor(*img);
      break;
    }
    emit processedImage(&result);
}

void Processor::setCount(int count)
{
    m_count = count;
}

int Processor::thickness() const
{
    return m_thickness;
}

void Processor::setThickness(int thickness)
{
    m_thickness = thickness;
}

int Processor::threshold() const
{
    return m_threshold;
}

void Processor::setThreshold(int threshold)
{
    m_threshold = threshold;
}

bool Processor::clicked() const
{
    return m_clicked;
}

void Processor::setClicked(bool clicked)
{
    m_clicked = clicked;
}

int Processor::count() const
{
    return m_count;
}

int Processor::getAlgo() const
{
    return m_algo;
}

void Processor::setAlgo(int algo)
{
    m_algo = algo;
}


