#include <iostream>
#include <opencv2/opencv.hpp>
#include <stack>

using namespace std;
using namespace cv;

const int th = 200;
const int maxRegion = 100;
const double minAreaFactor = 0.001;


const Point pointShift2D[8] =
{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1),                Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1,1)
};

void grow(const Mat& src, Mat& dst, Mat& mask, Point seed){

    stack<Point> pointStack;
    pointStack.push(seed);

    while( !pointStack.empty() ){

        Point center = pointStack.top();
        mask.at<uchar>(center) = 1;
        pointStack.pop();

        for(int i = 0; i < 8; i++){

            Point estimatingPoint = center + pointShift2D[i];

            if(estimatingPoint.x < 0 ||
               estimatingPoint.y < 0 ||
               estimatingPoint.x > src.cols-1 ||
               estimatingPoint.y > src.rows-1){

                    continue;
               }
            else{

                int delta = (int)(  pow(src.at<Vec3b>(seed)[0] - src.at<Vec3b>(estimatingPoint)[0], 2) 
                                  + pow(src.at<Vec3b>(seed)[1] - src.at<Vec3b>(estimatingPoint)[1], 2) 
                                  + pow(src.at<Vec3b>(seed)[2] - src.at<Vec3b>(estimatingPoint)[2], 2));
                
                if(dst.at<uchar>(estimatingPoint) == 0 &&
                   mask.at<uchar>(estimatingPoint) == 0 &&
                   delta < th){

                        pointStack.push(estimatingPoint);
                   }
            }
            
        }

    }
    
}

void RegionGrowing(const Mat& src, Mat& dst, int maxRegion, double minAreaFactor){

    int minRegionA = (int)(minAreaFactor * src.rows * src.cols);

    dst = Mat::zeros(src.size(), CV_8UC1);
    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    uchar label = 1;

    for(int x = 0; x < src.cols; x++){
        for(int y = 0; y < src.rows; y++){

            if(dst.at<uchar>(Point(x,y)) == 0){

                grow(src, dst, mask, Point(x,y));
                int maskArea = (int)sum(mask).val[0];

                if(maskArea > minRegionA){
                    dst = dst + mask*label;

                    imshow("Mask", mask*255);
                    waitKey(0);

                    if(++label > maxRegion){
                        cout<<"Out of Region"<<endl;
                        exit;
                    }
                }
                else{
                    dst = dst + mask*255;
                }

                mask -= mask;
            }
            
        }
    }

}

int main(int argc, char** argv){
    
    Mat src, dst;
    src = imread(argv[1]);

    if( src.empty() )
        return -1;


    if (src.cols > 500 || src.rows > 500) {
        resize(src, src, Size(0, 0), 0.5, 0.5); // resize for speed 
    }

    imshow("Original Img", src);
    waitKey(0);

    //GaussianBlur(src, src, Size(7,7), 0, 0);
    RegionGrowing(src, dst, maxRegion, minAreaFactor);

    imshow("RegionGr", dst);
    waitKey(0);

    return 0;
}