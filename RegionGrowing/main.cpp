#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main(int argc, char** argv){
    
    Mat src, dst;
    src = imread(argv[1], IMREAD_GRAYSCALE);
    
    if( src.empty() )
        return -1;

    imshow("Original Img", src);



    return 0;
}