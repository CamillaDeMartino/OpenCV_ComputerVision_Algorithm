#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat src, dst;

void harris(const Mat& src, Mat& dst, int ht = 150){

    Mat dX, dY;

    Sobel(src, dX, CV_32FC1, 1, 0);
    Sobel(src, dY, CV_32FC1, 0, 1);

    Mat dX2, dY2, dXY;
    pow(dX, 2, dX2);
    pow(dY, 2, dY2);
    multiply(dX, dY, dXY);

    Mat C00, C10, C01, C11;
    GaussianBlur(dX2, C00, Size(3,3), 0, 0);
    GaussianBlur(dY2, C11, Size(3,3), 0, 0);
    GaussianBlur(dXY, C10, Size(3,3), 0, 0);
    C10.copyTo(C01);

    Mat det, traccia, C1, C2;
    double k = 0.05;
    multiply(C00, C11, C1);
    multiply(C10, C01, C2);

    det = C1 - C2;
    traccia = C00 + C11;

    // R =  determinante - k*traccia^2
    Mat R;
    pow(traccia, 2, traccia);
    R = det - k * traccia;

    normalize(R, dst, 0, 255, NORM_MINMAX, CV_8UC1);

    Mat dst_scale;
    convertScaleAbs(dst, dst_scale);

    for(int i = 1; i < dst.rows-1; i++){
        for(int j = 1; j < dst.cols-1; j++){
            if(dst.at<uchar>(i,j) > ht)
                circle(dst_scale, Point(j,i), 5, Scalar(0), 2, 8, 0);
        }
    }

    imshow("Harris", dst_scale);
}


int main(int argc, char **argv){

    src = imread(argv[1], IMREAD_GRAYSCALE);
    if( src.empty() )
        return -1;

    imshow("My Img", src);
    waitKey(0);


    harris(src, dst);
    waitKey(0);


    return 0;
}