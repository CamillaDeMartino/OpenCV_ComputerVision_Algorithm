#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat src, dst;

void convertPolar(double rho, double theta, Point& p1, Point& p2){

    int x = cvRound(rho * cos(theta));
    int y = cvRound(rho * sin(theta));

    int alpha = 1000;
    p1.x = cvRound(x + alpha * (-sin(theta)));
    p1.y = cvRound(y + alpha * (cos(theta)));

    p2.x = cvRound(x - alpha * (-sin(theta)));
    p2.y = cvRound(y - alpha * (cos(theta)));
}

void hough(const Mat& src, Mat& dst, int ht = 140){

    Mat blur, imgCanny;
    GaussianBlur(src, blur, Size(3,3), 0, 0);
    Canny(blur, imgCanny, 100, 240, 3);

    int maxDiag = hypot(imgCanny.rows, imgCanny.cols);
    Mat votes = Mat::zeros(maxDiag*2, 180, CV_8UC1);

    double theta, rho;
    double rad = CV_PI/180;  
    
    for(int x = 0; x < imgCanny.rows; x++){
        for(int y = 0; y < imgCanny.cols; y++){
            if(imgCanny.at<uchar>(x,y) == 255){
                for(theta = 0; theta < 180; theta++){
                    rho = cvRound(x*sin((theta-90) * rad) + y*cos((theta-90) * rad))+ maxDiag;
                    votes.at<uchar>(rho,theta)++;
                }
            }
        }
    }

    dst = src.clone();
    Point p1, p2;
    vector<pair<Point, Point>> lines;

    for( int r = 0; r < votes.rows; r++){
        for(int t = 0; t < 180; t++){
            if(votes.at<uchar>(r,t) > ht){
                rho = r - maxDiag;
                theta = (t-90) * rad;

                convertPolar(rho, theta, p1, p2);
                lines.push_back(make_pair(p1,p2));
            }
        }
    }

    for(int i = 0; i < lines.size(); i++){
        pair<Point, Point> coordinates = lines.at(i);
        line(dst, coordinates.first, coordinates.second, Scalar(0, 0 , 255), 1, LINE_AA);
    }
}

int main(int argc, char** argv){

    src = imread(argv[1], IMREAD_COLOR);

    if( src.empty() )
        return -1;

    imshow("Original Img", src);
    waitKey(0);

    hough(src, dst);
    imshow("Hough", dst);
    waitKey(0);

    return 0;
}