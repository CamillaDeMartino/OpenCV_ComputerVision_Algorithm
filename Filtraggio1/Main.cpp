/**
 * Implementare l’algoritmo di correlazione/convoluzione e confrontare l’output con quello
 * prodotto dalla funzione filter2D() usando un filtro media
*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Convoluzione.cpp"

using namespace cv;

/**
* |1 2 1| 
* |2 4 2| -> la somma di questo filtro è 16, quindi dobbiamo dividere ogni valore per 16
* |1 2 1|
*   
*  1/16 -> 1/16. 
*  2/16 -> 1/8. 
*  4/16 -> 1/4. 
*/
int main(int argc, char** argv){

    Mat img = imread(argv[1], IMREAD_GRAYSCALE);
    if( img.empty() )
        return -1;


    //Mat mask = Mat::ones(3,3, CV_32F)/(9);   //filtro box
    
    //filtro media
    Mat filtroMedia = (Mat_<float>(3,3) << 1/16.0, 1/8.0, 1/16.0,
									       1/8.0,  1/4.0, 1/8.0,
										   1/16.0, 1/8.0, 1/16.0);
    Mat filter2Dimg;
    filter2D(img, filter2Dimg, img.type(), filtroMedia);

    Mat corrImg = correlazione(img, filtroMedia);
    Mat convImg = convoluzione(img, filtroMedia);

    imshow("original", img);
    waitKey(0);
    imshow("filter2D", filter2Dimg);
    waitKey(0);
    imshow("correlazione", corrImg);
    waitKey(0);
    imshow("convoluzione", convImg);
    waitKey(0);


    return 0;
}