#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int ht = 135;

void convertPolar(double rho, double theta, Point& p1, Point& p2){

    //trasformiamo le coordinate polari in coordinate cartesiane
    int x0 = cvRound(rho * cos(theta));
    int y0 = cvRound(rho * sin(theta));


    /**
     * I due punti pt1 e pt2 dovranno essere spostati lungo la retta (per questo moltiplichiamo
     * per alpha) ma in direzioni opposte dalla posizione iniziale (per questo in pt1 sommiamo mentre in pt2 sottraiamo).
    */
    int alpha = 1000;

    p1.x = cvRound(x0 + alpha * (-sin(theta)));
    p1.y = cvRound(y0 + alpha * (cos(theta)));

    p2.x = cvRound(x0 - alpha * (-sin(theta)));
    p2.y = cvRound(y0 - alpha * (cos(theta)));
}


void hough(const Mat &src, Mat &dst){

    //Blur dell'immagine
    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    //Trovare gli edge con Canny
    Mat imgCanny;
    Canny(blur, imgCanny, 80, 300, 3);
    imshow("Canny", imgCanny);
    waitKey(0);


    //Creiamo lo spazio dei voti, che sarà inizialmente una matrice di tutti 0,
    //Questo corrisponde a considerare una possibile distanza ρ (che andrà da ρ a – ρ) (è la diagonale calcolata con hypot) 
    //e dei possibili angoli θ (180°), quindi la nostra quantizzazione avrà ρ unità e considererà θ angoli.

    int maxDist = hypot(imgCanny.rows, imgCanny.cols);
    Mat votes = Mat::zeros(maxDist*2, 180, CV_8UC1);


    /**
     * Effettuiamo la scansione dell’immagine di Canny. 
     * Ci interessano solo i punti di edge, ovvero tutti quei punti il quale valore è 255.
     * Questi punti voteranno nello spazio di voti.
     * Per votare i pixel di edge dovranno tener conto di tutte le possibili direzioni, che in questo caso sarebbero tutte quelle comprese in un angolo da -90° a 90°.
     * Nell’istruzione seguente calcoliamo il rho tenendo conto della forma normale:
     *      rho = round(y*cos(theta-90) + x*sin(theta-90)) + maxDist;
     * nelle funzioni sin e cos sottraggo 90 a theta, così da ritrovarmi nell’intervallo [-90,90].
     * 
     * Nb: il motivo per cui x e y sono invertite rispetto alla formula originale è perché in realtà non siamo nel 1° quadrante ma nel 4°
     * 
     * Nb: al valore calcolato dalla formula, dobbiamo aggiungere maxDist, sarà la nostra costante da sottrarre nella conversione polare per trovare il giusto rho ad ogni iterazione. 
     * 
     * Dunque, per ogni singola iterazione fisso un angolo theta, calcolo il rho, 
     * e per questa coppia (ro,theta) inserisco il voto nello spazio dei voti.
    */

    double rho;
    double theta;

    for(int x = 0; x < imgCanny.rows; x++){
        for(int y = 0; y < imgCanny.cols; y++){
            if( imgCanny.at<uchar>(x,y) == 255 ){
                for( theta = 0; theta < 180; theta++){
                    rho = cvRound(y*cos((theta-90)*CV_PI/180) + x*sin((theta-90)*CV_PI/180)) + maxDist;       //*CV_PI/180 è per la conversione da radianti
                    votes.at<uchar>(rho, theta)++;
                }
            }
        }
    }


    /**
     * Terminato il voto, dovrò andare a considerare lo spazio di voti.
     * Dovrò estrarre dallo spazio di voto le celle che hanno un numero di voti maggiore di una certa soglia (Hth).
     * Il risultato verrà inserito in un’altra matrice che infine andremo a restituire.
     * 
     * Dovrò portare rho al suo valore originale. 
     * Ricordiamo che il valore di rho originale sarebbe quello dato dalla formula della retta normale.
     * questo significa che per ottenere quel valore dobbiamo sottrarre maxDist a r.
     * 
     * Dovrò portare theta in un’intervallo compreso fra [-90,90], dunque dobbiamo sottrarre 90 a t.
    */

    Point p1, p2;
    dst = src.clone();

    vector<pair<Point, Point>> lines;
    cvtColor(dst, dst, COLOR_GRAY2BGR);

    for(int r = 0; r < votes.rows; r++){
        for(int t = 0; t < 180; t++){
            if(votes.at<uchar>(r, t) > ht){
                rho = r - maxDist;
                theta = (t - 90) * CV_PI/180;

                convertPolar(rho, theta, p1, p2);
                lines.push_back(make_pair(p1,p2));
            }
        }
    }



    for(int i = 0; i < lines.size(); i++){
        pair<Point, Point> coordinates = lines.at(i);
        line(dst, coordinates.first, coordinates.second, Scalar(0, 0, 255), 1, LINE_AA);

    }
}




int main(int argc, char** argv){

    Mat src;
    Mat dst;

    src = imread(argv[1], IMREAD_GRAYSCALE);
     
    if( src.empty() )
        return -1;

    imshow("Orginal img", src);
    waitKey(0);


    hough(src, dst);
    imshow("Hough", dst);
    waitKey(0);

    return 0;
}