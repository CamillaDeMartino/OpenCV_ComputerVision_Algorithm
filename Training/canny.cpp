#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat src, dst;

void canny(const Mat& src, Mat& dst, int ht = 100, int lt = 30){

    //sfoca
    Mat blur, dX, dY;
    GaussianBlur(src, blur, Size(5,5), 0, 0);

    Sobel(blur, dX, src.type(), 1, 0);
    Sobel(blur, dY, src.type(), 0, 1);

    //magnitudo
    Mat magn;
    magn = abs(dX) + abs(dY);
    normalize(magn, magn, 0, 255, NORM_MINMAX, CV_8UC1);

    //angolo di fase
    Mat orientation;
    dX.convertTo(dX, CV_32FC1);
    dY.convertTo(dY, CV_32FC1);

    phase(dX, dY, orientation, true);

    //no maxima suppression
    Mat maxSupp;
    magn.copyTo(maxSupp);
    copyMakeBorder(maxSupp, maxSupp, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));


    for(int i = 1; i < maxSupp.rows-1; i++){
        for(int j = 1; j < maxSupp.cols-1; j++){

            float ang = orientation.at<float>(i-1,j-1) > 180 ? orientation.at<float>(i-1,j-1) - 180 : orientation.at<float>(i-1,j-1);

            //orizzontale
            if(0 <= ang && ang <= 22.5 || 157.5 < ang && ang <= 180 ){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i, j - 1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i, j + 1))
                    maxSupp.at<uchar>(i,j) = 0;
            }
            //+45
            else if(22.5 < ang && ang <= 67.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1, j-1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1, j +1 ))
                    maxSupp.at<uchar>(i,j) = 0;
            }
            //verticale
            else if(67.5 < ang && ang <= 112.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1, j) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1, j))
                    maxSupp.at<uchar>(i,j) = 0;
            }
            //-45
            else if(112.5 < ang && ang <= 157.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1, j + 1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1, j - 1))
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


                if(strong_e)
                    maxSupp.at<uchar>(i,j) = 255;
                else 
                    maxSupp.at<uchar>(i,j) = 0;
            }
        }
    }

    maxSupp.copyTo(dst);

}


int main(int argc, char** argv){


    src = imread(argv[1], IMREAD_GRAYSCALE);

    if( src.empty() )
        return -1;

    imshow("Original", src);

    canny(src, dst);
    imshow("Canny", dst);
    waitKey(0);
    
    return 0;
}