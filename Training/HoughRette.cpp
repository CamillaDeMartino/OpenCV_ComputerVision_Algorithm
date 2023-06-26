#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat src, dst;
int ht = 135;

void polarConvert(double rho, double theta, Point& p1, Point& p2){

    int x0, y0;

    x0 = cvRound(rho * cos(theta));
    y0 = cvRound(rho * sin(theta));

    int alpha = 1000;
    p1.x = cvRound(x0 + alpha * (-sin(theta)));
    p1.y = cvRound(y0 + alpha * (cos(theta)));

    p2.x = cvRound(x0 - alpha * (-sin(theta)));
    p2.y = cvRound(y0 - alpha * (cos(theta)));

}


void hough(const Mat& src, Mat& dst){

    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    Mat cannyImg;
    Canny(blur, cannyImg, 80, 300, 3);

    Mat votes;
    int diagMax = hypot(cannyImg.rows, cannyImg.cols);
    votes = Mat::zeros(diagMax*2, 180, CV_8UC1);

    double theta, rho;
    for(int x = 0; x < cannyImg.rows; x++){
        for(int y = 0; y < cannyImg.cols; y++){
            if(cannyImg.at<uchar>(x,y) == 255){
                for(theta = 0; theta < 180; theta++){
                    rho = cvRound(y * cos((theta-90) * CV_PI/180) + x * sin((theta-90) * CV_PI/180)) + diagMax;
                    votes.at<uchar>(rho, theta)++;
                }
            }
        }
    }


    dst = src.clone();
    cvtColor(dst, dst, COLOR_GRAY2BGR);

    Point p1, p2;
    vector<pair<Point, Point>> lines;

    for(int r = 0; r < votes.rows; r++){
        for(int t = 0; t < 180; t++){
            if(votes.at<uchar>(r,t) > ht){
                rho = r - diagMax;
                theta = (t - 90) * CV_PI/180;

                polarConvert(rho, theta, p1, p2);
                lines.push_back(make_pair(p1,p2));
            }
        }
    }

    for(int i = 0; i < lines.size(); i++){
        pair<Point, Point> coordinates = lines.at(i);
        line(dst, coordinates.first, coordinates.second, Scalar(0, 0, 255), 1, LINE_AA);

    }
}

int main(int argc, char** argv){

    src = imread(argv[1], IMREAD_GRAYSCALE);

    if( src.empty() )
        return -1;

    imshow("Original Img", src);

    hough(src, dst);
    imshow("Hough", dst);
    waitKey(0);

    return 0;
}