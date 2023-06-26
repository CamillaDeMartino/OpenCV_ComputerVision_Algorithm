#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat src, dst;
int ht = 100, lt = 30;

void canny(const Mat& src, Mat& dst){

    //blur
    Mat blur;
    GaussianBlur(src, blur, Size(3, 3), 0, 0);

    //Magnitudo
    Mat sobelX, sobelY, magn;
    Sobel(blur, sobelX, src.type(), 1, 0);
    Sobel(blur, sobelY, src.type(), 0, 1);
    magn = abs(sobelX) + abs(sobelY);


    //angolo di fase
    Mat orientation;
    sobelX.convertTo(sobelX, CV_32FC1);
    sobelY.convertTo(sobelY, CV_32FC1);
    phase(sobelX, sobelY, orientation, true);

    //no maxima suppress
    Mat maxSupp, magnNorm;
    normalize(magn, magnNorm, 0, 255, NORM_MINMAX, CV_8UC1);
    magnNorm.copyTo(maxSupp);

    copyMakeBorder(maxSupp, maxSupp, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));

    for(int i = 1; i < magnNorm.rows-1; i++){
        for(int j = 1; j < magnNorm.cols-1; j++){

            float ang = orientation.at<float>(i,j) > 180 ? orientation.at<float>(i,j) - 360 : orientation.at<float>(i,j);

            //orizz
            if(-22.5 < ang && ang <= 22.5 || 157.5 < ang && ang <= -157.5){
                if( maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i, j-1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i, j+1))
                    maxSupp.at<uchar>(i,j) = 0;
            }
            //vert
            else if(67.5 < ang && ang <= 112.5 || -112.5 < ang && ang <= -67.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i-1, j) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i+1, j)) 
                    maxSupp.at<uchar>(i,j) = 0;
            }
            //+45
            else if(112.5 < ang && ang <= 157.5 || -67.5 < ang && ang <= -22.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i+1, j-1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i-1, j+1)) 
                    maxSupp.at<uchar>(i,j) = 0;
            }
            //-45
            else if(22.5 < ang && ang <= 67.5 || -157.5 < ang && ang <= -112.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i-1, j-1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i+1, j+1)) 
                    maxSupp.at<uchar>(i,j) = 0;
            }
        }
    }


    //threshold
    for(int i = 1; i < maxSupp.rows-1; i++){
        for(int j = 1; j < maxSupp.cols-1; j++){
            if(maxSupp.at<uchar>(i,j) > ht)
                maxSupp.at<uchar>(i,j) = 255;
            else if(maxSupp.at<uchar>(i,j) < lt)
                maxSupp.at<uchar>(i,j) = 0;
            else if(maxSupp.at<uchar>(i,j) <= ht && maxSupp.at<uchar>(i,j) >= lt){
                bool strong_e = false;
                for(int l = -1; l <= 1 && !strong_e; l++){
                    for(int k = -1; k <= 1 && !strong_e; k++){
                        if(maxSupp.at<uchar>(i+l,j+k) > ht)
                            strong_e = true;
                    }
                }

                if(strong_e){
                    maxSupp.at<uchar>(i,j) = 255;
                }
                else
                    maxSupp.at<uchar>(i,j) = 0;
            }
        }
    }

    maxSupp.copyTo(dst);

}

void CannyThreshold(int, void*){
    canny(src, dst);
    imshow("Canny", dst);
}

int main(int argc, char** argv){

    src = imread(argv[1], IMREAD_GRAYSCALE);
    if( src.empty())
        return -1;

    imshow("original", src);
    waitKey(0);

    namedWindow("Canny");
    createTrackbar("Trackbar ht", "Canny", &ht, 255, CannyThreshold);
    createTrackbar("Trackbar lt", "Canny", &lt, 255, CannyThreshold);
    CannyThreshold(0,0);
    waitKey(0);

    return 0;
}