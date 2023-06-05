/**
 * il padding ci serve perché senza di esso faremmo un accesso illegare alla memoria quando andiamo a calcolare degli intorni: 
 * si pensi al primo pixel (0,0), se vogliamo calcolare un intorno 3x3 ci accorgeremo che sopra e a sinistra non abbiamo pixel, 
 * e ciò comporterà un errore; tuttavia se aggiungiamo un padding (pixel fantoccio) a quei lati, potremo calcolare l’intorno.
 * Se l’intorno è 3x3 ne basta 1, se 5x5 allora 2, se 7x7 allora 3, ecc.
*/
#include <opencv2/opencv.hpp>
#include <iostream>


using namespace std;
using namespace cv;

/**
 * In subMat3x3, inizializziamo a 0 (nero) la somma sum, di tipo Vec3i. 
 * Il motivo per cui usiamo un vector di interi (e non byte) e perché i byte hanno valore massimo 255, dunque andremmo subito in overflow.
 * Usiamo un doppio ciclo for dove sommiamo tutti gli elementi di una sottomatrice 3x3 che parte da (istart,jstart).
 * Infine ritorniamo sum, effettuando un cast a Vec3b (perché è il tipo che vogliamo ritornare).
 * 
 * 
 * Il motivo per cui istart e jstart partono da (-1) è perché vogliamo considerare il “centro del quadrato 3x3”, 
 * e dunque se (i,j) rappresentano il centro, allora (i-1, j-1) rappresentano l’angolo in alto a sx.
*/
Vec3b subMat3x3Color(Mat img, int istart, int jstart){
    Vec3i sum(0, 0, 0);

    for(int i = istart-1; i < istart+2; i++)
        for(int j = jstart-1; j < jstart+2; j++)
            sum += img.at<Vec3b>(i,j);

    return Vec3b(sum/9);

}

unsigned char subMat3x3BW(Mat img, int istart, int jstart){
    int sum = 0;

    for(int i = istart-1; i < istart+2; i++)
        for(int j = jstart-1; j < jstart+2; j++)
            sum += img.at<unsigned char>(i,j);

    return (unsigned char)(sum/9);

}

int main( int argc, char** argv ){

    /**
     * 1. creazione dell’immagine con padding. Aggiungeremo 1 vector di pixel neri ad ogni lato;
     * 2. definiamo la matrice result. Ovviamente avrà lo stesso numero di righe e colonne, e lo stesso tipo dell’immagine
     * 3. Effettuiamo il calcolo. Usiamo un doppio ciclo for perché vogliamo iterare su ogni elemento della matrice.
     * 
     * Se che vogliamo fare questa operazione su un’immagine a colori, ogni pixel sarà composto da 3 valori (BGR), -> Scalar value(0,0,0) 
     * e i pixel li  dovremmo rappresentare più come Vec3b (3 canali, byte).
     * Il valore del pixel nell’intorno 3x3 sarà calcolato attraverso la funzione submean3x3.
    */


    Mat img = imread(argv[1], IMREAD_GRAYSCALE);
    if( img.empty() )
        return -1;

    namedWindow("Example1", WINDOW_AUTOSIZE);
    imshow("Example1", img);
    waitKey(0);
    cout<<img.size()<<endl;
    

    Mat imgPadded;
    //Scalar value(0, 0, 0);      //pixel neri si volessimo una versione a colori
    Scalar value(0);              //pixel neri
    copyMakeBorder(img, imgPadded, 1, 1, 1, 1, BORDER_CONSTANT);         //Se l’intorno è 3x3 ne basta aggiungere 1 a tutti i lati
    namedWindow("Example2", WINDOW_AUTOSIZE);
    imshow("Example2", imgPadded);
    waitKey(0);
    cout<<imgPadded.size()<<endl;

    Mat result(img.rows, img.cols, img.type());

    int i,j;
    for(i = 1; i < imgPadded.rows-1; i++)
        for(j = 1; j < imgPadded.cols-1; j++)
            result.at<unsigned char>(i-1, j-1) = subMat3x3BW(imgPadded, i, j); 
            //result.at<Vec3b>(i-1, j-1) = subMat3x3Color(imgPadded, i, j);       //se lo volessimo a colori


    namedWindow("Example3", WINDOW_AUTOSIZE);
    imshow("Example3", result);
    waitKey(0);
    

    return 0;
}



