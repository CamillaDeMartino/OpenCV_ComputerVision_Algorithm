#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int ht = 110;
int Rmin = 30;
int Rmax = 150;
Mat src, dst;

void hough(const Mat& src, Mat& dst){

    Mat blur, imgCanny;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    Canny(blur, imgCanny, 100, 200, 3);

    int size[] = {imgCanny.rows, imgCanny.cols, Rmax-Rmin + 1};
    Mat votes = Mat::zeros(3, size, CV_8UC1);

    for(int x = 0; x < imgCanny.rows; x++){
        for(int y = 0; y < imgCanny.cols; y++){
            if(imgCanny.at<uchar>(x,y) == 255){
                for(int r = Rmin; r < Rmax; r++){
                    for(int t = 0; t < 360; t++){

                        int a = x - r * sin(t * CV_PI/180);
                        int b = y - r * cos(t * CV_PI/180);

                        if(a >= 0 && a < imgCanny.rows && b >= 0 && b < imgCanny.cols)
                            votes.at<uchar>(a,b,r)++;
                    }
                }
            }
        }
    }

    dst = src.clone();
    for(int r = Rmin; r < Rmax; r++){
        for(int x = 0; x < imgCanny.rows; x++){
            for(int y = 0; y < imgCanny.cols; y++){
                if(votes.at<uchar>(x,y,r) > ht){
                    circle(dst, Point(y,x), 3, Scalar(70, 70, 115), 2, 8, 0);
                    circle(dst, Point(y,x), r, Scalar(70, 70, 115), 2, 8, 0);
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

    hough(src, dst);
    imshow("Hough", dst);
    waitKey(0);



    return 0;

}