#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;

void createClusterInfo(const Mat& src, int clustersNum, vector<Scalar>& clustersCenters, vector<vector<Point>> &ptInClusters){

    RNG random(getTickCount());

    for(int k = 0; k < clustersNum; k++){

        Point pixel;
        pixel.x = random.uniform(0, src.cols);
        pixel.y = random.uniform(0, src.rows);

        Scalar pixelCenter = src.at<Vec3b>(pixel.y, pixel.x);
        Scalar pixelColor(pixelCenter.val[0], pixelCenter.val[1], pixelCenter.val[2]);

        clustersCenters.push_back(pixelColor);

        vector<Point> ptInCluster;
        ptInClusters.push_back(ptInCluster);
    }
}

double colorDistance(Scalar pixel, Scalar pixelCluster){

    double blue = pixel.val[0] - pixelCluster.val[0];
    double green = pixel.val[1] - pixelCluster.val[1];
    double red = pixel.val[2] - pixelCluster.val[2];

    double distance = sqrt(pow(blue, 2) + pow(green, 2) + pow(red, 2));

    return distance;
}

void findKCluster(const Mat& src, int clustersNum, vector<Scalar> clustersCenters, vector<vector<Point>> &ptInClusters){

    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){

            double minDistance = INFINITY;
            double indexClosestCluster = 0;
            Scalar pixel = src.at<Vec3b>(x,y);

            for(int k = 0; k < clustersNum; k++){

                double distance  = colorDistance(pixel, clustersCenters[k]);

                if(distance < minDistance){
                    minDistance = distance;
                    indexClosestCluster = k;
                }

            }

            ptInClusters[indexClosestCluster].push_back(Point(y,x));
        }
    }
}

double fixClusterCenter(const Mat& src, int clustersNum, vector<Scalar> &clustersCenters, vector<vector<Point>> &ptInClusters, double &oldValue, double newValue){

    double diffChange;

    for(int k = 0; k < clustersNum; k++){

        double newBlue = 0;
        double newGreen = 0;
        double newRed = 0;

        vector<Point> ptInClusterk = ptInClusters[k];

        for(int i = 0; i < ptInClusterk.size(); i++){

            Scalar pixel = src.at<Vec3b>(ptInClusterk[i].y, ptInClusterk[i].x);
            newBlue += pixel.val[0];
            newGreen += pixel.val[1];
            newRed += pixel.val[2];

        }

        newBlue /= ptInClusterk.size();
        newGreen /= ptInClusterk.size();
        newRed /= ptInClusterk.size();

        Scalar newPixel(newBlue, newGreen, newRed);
        newValue += colorDistance(newPixel, clustersCenters[k]);
        clustersCenters[k] = newPixel;
    }

    newValue /= clustersNum;
    diffChange = abs(oldValue - newValue);
    oldValue = newValue;

    return diffChange;
}

void applyFinalChange(Mat& dst, int clustersNum, vector<Scalar> &clustersCenters, vector<vector<Point>> &ptInClusters){

    for(int k = 0; k < clustersNum; k++){

        Scalar pixel = clustersCenters[k];
        vector<Point> ptInClusterk = ptInClusters[k];
        
        for(int i = 0; i < ptInClusterk.size(); i++){

            dst.at<Vec3b>(ptInClusterk[i])[0] = pixel.val[0];
            dst.at<Vec3b>(ptInClusterk[i])[1] = pixel.val[1];
            dst.at<Vec3b>(ptInClusterk[i])[2] = pixel.val[2];
        }
        
    }
}

int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_COLOR);

    if(src.empty())
        return -1;

    imshow("Original img", src);

    int clustersNum = stoi(argv[2]);

    double th = 0.1;
    double oldValue = INFINITY;
    double newValue = 0;
    double diffChange = oldValue - newValue;
    vector<Scalar> clustersCenter;
    vector<vector<Point>> ptInClusters;

    createClusterInfo(src, clustersNum, clustersCenter, ptInClusters);
    Mat dst = src.clone();

    while(diffChange > th){

        newValue = 0;

        for(int k = 0; k < clustersNum; k++){
            ptInClusters[k].clear();
        }

        findKCluster(src, clustersNum, clustersCenter, ptInClusters);

        diffChange = fixClusterCenter(src, clustersNum, clustersCenter, ptInClusters, oldValue, newValue);
    }

    applyFinalChange(dst, clustersNum, clustersCenter, ptInClusters);
    imshow("dst", dst);
    waitKey(0);

    return 0;
}