#include <iostream>
#include <opencv2/opencv.hpp>
#include "Correlazione.cpp"
#include "CalcFilteredValue2.cpp"

using namespace cv;

Mat convoluzione(Mat src, Mat mask){
    Mat tmp;

    rotate(mask, tmp, ROTATE_180);

    Mat matOutput(src.rows, src.cols, src.type());

    Mat pad;
    int bw = mask.rows/2;

    copyMakeBorder(src, pad, bw, bw, bw, bw, BORDER_DEFAULT);

    for( int i = 0; i < matOutput.rows; i++ )
        for( int j = 0; j < matOutput.cols; j++ )
            matOutput.at<uchar>(i, j) = filteredValue2(pad, tmp, i, j);

    return matOutput;
    //return correlazione(tmp, mask);

}