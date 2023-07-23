#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int Rmin = 30;
const int Rmax = 150;
const int ht = 140;

void houghC(const Mat& src, Mat& dst){

    Mat blur, imgCanny;
    GaussianBlur(src, blur, Size(3,3), 0, 0);
    Canny(blur, imgCanny, 100, 200);

    int size[] = {imgCanny.rows, imgCanny.cols, (Rmax-Rmin) + 1};
    Mat votes = Mat::zeros(3, size, CV_8UC1);

    for(int x = 0; x < imgCanny.rows; x++){
        for(int y = 0; y < imgCanny.cols; y++){
            if(imgCanny.at<uchar>(x,y) == 255){
                for(int r = Rmin; r < Rmax; r++){
                    for(int t = 0; t < 360; t++){

                        int a = cvRound(x - r*sin(t * CV_PI/180));
                        int b = cvRound(y - r*cos(t * CV_PI/180));

                        if(a >= 0 && a < imgCanny.rows && b >= 0 && b < imgCanny.cols)
                            votes.at<uchar>(a,b,r-Rmin)++;
                    }
                }
            }
        }
    }

    src.copyTo(dst);
    for(int r = Rmin; r < Rmax; r++){
        for(int x = 0; x < imgCanny.rows; x++){
            for(int y = 0; y < imgCanny.cols; y++){

                if(votes.at<uchar>(x,y,r-Rmin) > ht){

                    circle(dst, Point(y,x), 3, Scalar(127, 127, 0), 2, 8, 0);
                    circle(dst, Point(y,x), r, Scalar(127, 127, 0), 2, 8, 0);
                }
            }
        }
    }
}



int main(int argc, char** argv){

    Mat src, dst;
    src = imread(argv[1], IMREAD_COLOR);

    if( src.empty() )
        return -1;

    imshow("Original img", src);

    houghC(src, dst);
    imshow("Hoigh", dst);
    waitKey(0);


    return 0;

}