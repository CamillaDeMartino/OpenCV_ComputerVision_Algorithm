/**
* Implementare il Laplaciano con kernel isotropico a 45° e 90° utilizzando la funzione di correlazione/convoluzione (o filter2D())
*    - Per normalizzare i livelli di grigio è possibile usare la funzione normalize()
*    - normalize(src, dst, 0, 255, NORM_MINMAX, CV_8U);
*
* Implementare il filtro di Sobel (gx e gy) utilizzando la funzione di correlazione/convoluzione (o filter2D())
*    - Calcolare la risposta di entrambi i filtri
*    - Calcolare la magnitudo del gradiente (entrambe le formulazioni)
*    - Utilizzare la risposta ottenuta per effettuare lo sharpening di un'immagine
*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Correlazione.cpp"

using namespace cv;

int main(int argc, char** argv){

    Mat img = imread(argv[1], IMREAD_GRAYSCALE);
    if( img.empty() )
        return -1;

    Mat laplaciano45 = (Mat_<float>(3,3) << 1.0, 1.0, 1.0,
									        1.0, -8.0, 1.0,
										    1.0,  1.0, 1.0);


    Mat laplaciano90 = (Mat_<float>(3,3) << 0.0, 1.0, 0.0,
									        1.0, -4.0, 1.0,
										    0.0,  1.0, 0.0);


    Mat filter45, filter90;
    filter2D(img, filter45, img.type(), laplaciano45);
    filter2D(img, filter90, img.type(), laplaciano90);

    Mat lapImg45;
    Laplacian(img,lapImg45,img.type(),3);        //3 = laplaciano a 45°
    Mat lapImg90;
    Laplacian(img,lapImg90,img.type(),1);        //1 = laplaciano a 90°

    imshow("Filtro2D_45", filter45);
    waitKey(0);
    imshow("Filtro2D_90", filter90);
    waitKey(0);
    imshow("Laplaciano_90", lapImg90);
    waitKey(0);
    imshow("Laplaciano_45", lapImg45);
    waitKey(0);


    Mat corrImg45 = correlazione(img, laplaciano45); 
    imshow("Corr45", corrImg45);
    waitKey(0); 

    Mat normalize1;
    normalize(corrImg45, normalize1, 0, 255, NORM_MINMAX, img.type());
    imshow("Corr45_Normalized", normalize1);
    waitKey(0); 

    Mat corrImg90 = correlazione(img, laplaciano90); 
    imshow("Corr90", corrImg90);
    waitKey(0); 

    Mat normalize2;
    normalize(corrImg90, normalize2, 0, 255, NORM_MINMAX, img.type());
    imshow("Corr90_Normalized", normalize2);
    waitKey(0); 



    return 0;
}