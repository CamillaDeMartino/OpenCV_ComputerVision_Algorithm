#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat src, dst;
int ht = 68;

void harris(const Mat& src, Mat& dst){

    //1. Calcolare le derivate  rispetto x e y
    Mat dX, dY;
    Sobel(src, dX, CV_32FC1, 1, 0);
    Sobel(src, dY, CV_32FC1, 0, 1);

    //2. Calcolare dX^2, dY^2 e dX*dY
    Mat dX2, dY2, dXY;
    pow(dX, 2, dX2);
    pow(dY, 2, dY2);
    multiply(dX, dY, dXY);

    //3. applicare un filtro Gaussiano a dX2, dY2 e dXY; 
    //    Praticamente, è quello che facciamo quando moltiplichiamo quei valori per w(x,y).
    //    Otteniamo le componenti C00, C01, C10, C11
    Mat  C00, C01, C10, C11;
    copyMakeBorder(dX2, dX2, 1, 1, 1, 1, BORDER_REFLECT);
    GaussianBlur(dX2, C00, Size(3,3), 0, 0);
    copyMakeBorder(dY2, dY2, 1, 1, 1, 1, BORDER_REFLECT);
    GaussianBlur(dY2, C10, Size(3,3), 0, 0);
    copyMakeBorder(dXY, dXY, 1, 1, 1, 1, BORDER_REFLECT);
    GaussianBlur(dXY, C11, Size(3,3), 0, 0);
    C10.copyTo(C01);

    //4. Calcoliamo R = determinante - k*taccia^2
    //    Determinante = C00*C11 - C01*C10
    Mat C1, C2, det, traccia, R;
    double k = 0.05;
    multiply(C00, C11, C1);
    multiply(C10, C10, C2);
    det = C1 - C2;
    //    traccia =  C00 + C11
    traccia = C00 + C11;
    pow(traccia, 2, traccia);
    traccia = k*traccia;
    R = det - traccia;

    //5. Normalizzare R
    normalize(R, dst, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    

}

void HarrisThreshold(int, void*){
	harris(src,dst);

    Mat dst_scale;
    //normalize(dst, dst_scale, 0, 255, NORM_MINMAX, CV_32FC1,Mat());
    convertScaleAbs(dst, dst_scale);

    //6. Sogliare R
    for(int i = 1; i < dst.rows-1; i++){
        for(int j = 1; j < dst.cols-1; j++){
            if(dst.at<float>(i,j) > ht)
                circle(dst_scale, Point(j,i),5, Scalar(0), 2, 8, 0);
        }
    }
    imshow("Harris",dst_scale);

}

int main(int argc, char **argv){

    src = imread(argv[1], IMREAD_GRAYSCALE);
    if( src.empty() )
        return -1;

    imshow("My Img", src);
    waitKey(0);


    namedWindow("Harris");
    createTrackbar("Trackbar th", "Harris", &ht, 255, HarrisThreshold);
    HarrisThreshold(0,0);
    //harris(src, dst);
    //imshow( "Harris", dst);
    waitKey(0);

    Mat Harris;
    cornerHarris( src, Harris, 2, 3, 0.05);
    normalize( Harris, Harris, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( Harris, Harris );
    imshow( "Harris Cv", Harris);
    waitKey(0);


    return 0;
}

