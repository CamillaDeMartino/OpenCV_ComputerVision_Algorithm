#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat src, dst;
int ht = 100, lt = 30;

void canny(const Mat& src, Mat& dst){

    
}

int main(int argc, char** argv){

    src = imread(argv[1], IMREAD_GRAYSCALE);

    if( src.empty() )
        return -1;

    imshow("Original", src);

    
    return 0;
}