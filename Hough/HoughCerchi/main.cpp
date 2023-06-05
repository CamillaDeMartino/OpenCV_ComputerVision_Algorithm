#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int Rmax = 150; 
int Rmin = 30;
int houghTH = 100; 
int cannyLTH = 80; 
int cannyHTH = 300;

void hough(const Mat &src, Mat &dst){

    //Blur all'immagine per eliminare rumore
    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    //Canny oer individuare gli edge
    Mat imgCanny;
    Canny(blur, imgCanny, cannyLTH, cannyHTH, 3);
  

    /**
     * Creiamo lo spazio dei voti, che sarà tridimensionale. 
     * In questo caso i 3 parametri (a,b,R) saranno rappresentati rispettivamente da 
     * imgCanny.rows, imgCanny.cols e un intervallo di valori per R (consideriamo un min e un max).
     * Alla creazione la matrice sarà tutti 0.
    */

    int size[] = {imgCanny.rows, imgCanny.cols, (Rmax - Rmin) + 1 };
    //Mat votes(3, size, CV_32S, Scalar(0));
    Mat votes = Mat::zeros(3, size, CV_8UC1);
    
    /**
     * Effettuiamo la scansione dell’immagine di Canny. 
     * Ci interessano solo i punti di edge, ovvero tutti quei punti il quale valore è 255. Questi punti voteranno nello spazio di voti.
     * In questo caso abbiamo anche un terzo ciclo for che scorre anche per tutti i raggi compresi fra Rmin e Rmax.
    */
    //int a, b;
    for(int x = 0; x < imgCanny.rows; x++){
        for(int y = 0; y < imgCanny.cols; y++){
            if(imgCanny.at<uchar>(x, y) == 255){
                for(int r = Rmin; r < Rmax; r++){
                    for(int t = 0; t < 360; t++){

                        //a, b sono il centro del cerchio
                        int a = round(y - r*cos(t * CV_PI/180));
                        int b = round(x - r*sin(t * CV_PI/180));

                        //controllo che le coordinate siano interne all'immagine
                        if(a >= 0 && a < imgCanny.cols && b >= 0 && b < imgCanny.rows){
                            votes.at<uchar>(b,a,r)++;
                        }
                    }
                }
            }
        }
    }
    

    /**
     * Dovrò estrarre dallo spazio di voto le celle che hanno un numero di voti maggiore di una certa soglia (houghTH).
    */

    dst = src.clone();
    cvtColor(dst, dst, COLOR_GRAY2BGR);

    for(int r = Rmin; r < Rmax; r++){
        for(int x = 0; x < imgCanny.rows; x++){
            for(int y = 0; y < imgCanny.cols; y++){
                if(votes.at<uchar>(x,y,r) >= houghTH){
                    circle(dst, Point(y ,x), 3, Scalar(0, 0, 255), 2, 8, 0);
                    circle(dst, Point(y, x), r, Scalar(0, 0, 255), 2, 8, 0);
                }
            }
        }
    }
}



int main(int argc, char** argv){

    Mat src, dst;
    src = imread(argv[1], IMREAD_GRAYSCALE);

    if( src.empty() )
        return -1;

    imshow("Original img", src);

    hough(src, dst);
    imshow("Hough", dst);
    waitKey(0);



    return 0;

}