#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<float> normHistogram(const Mat& src){

    vector<float> hist(256, 0.0f);      //è vettore che rappresenta l'istogramma, è composto da 256 elementi float inizializzati a 0
    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){
            hist.at(src.at<uchar>(x,y))++;
        }
    }

    for(int i = 0; i < 256; i++){
        hist.at(i) /= src.rows*src.cols;
    }

    return hist;
}


/**
 * La media globale è calcolata sempre allo stesso modo, non dipende dalla kstar.
 * 
 * A differenza di prima, non possiamo più tenere in considerazione una sola probabilità prob1 (da cui poi derivavamo prob2), 
 * ma dobbiamo calcolare la probabilità ad ogni passo di ogni classe.
 * 
 * Discorso simile anche per quanto riguarda le medie cumulative.
*/

vector<int> otsu2K(const vector<float> hist){
    float globalAvg = 0.0f;

    for(int i = 0; i < 256; i++){
        globalAvg += i*hist.at(i);
    }

    //3 probabilità P1, P2, P3
    vector<float> prob(3, 0.0f);
    //3 medie cumulative m1, m2, m3
    vector<float> cumAvg(3, 0.0f);
    //2 soglie
    vector<int> kstar(2, 0);
    //massima distanza
    float maxVariance = 0.0f;

    for(int i = 0; i < 254; i++){
        //calcolo della probabilità P1(k)
        prob.at(0) += hist.at(i);
        //calcolo media cumulativa m1(k)
        cumAvg.at(0) += i * hist.at(i);

        for(int j = i+1; j < 255; j++){
            //calcolo della probabilità P2(k)
            prob.at(1) += hist.at(j);
            //calcolo media cumulativa m2(k)
            cumAvg.at(1) += j * hist.at(j);

            for(int k = j+1; k < 256; k++){
                //calcolo della probabilità P3(k)
                prob.at(2) += hist.at(k);
                //calcolo media cumulativa m3(k)
                cumAvg.at(2) += k * hist.at(k);

                //calcolo della varianza interclasse
                float sigma = 0.0f;
                for(int z = 0; z < 3; z++){
                    sigma += prob.at(z) * pow((cumAvg.at(z)/prob.at(z) - globalAvg), 2); 
                }

                //calcola la massima distanza
                if(sigma > maxVariance){
                    maxVariance = sigma;
                    kstar.at(0) = i;
                    kstar.at(1) = j;
                }
            }
            prob.at(2) = cumAvg.at(2) = 0.0f;
        }
        prob.at(1) = cumAvg.at(1) = 0.0f;
    }
    return kstar;
}

void myThreshold(const Mat src, Mat& dst, vector<int> kstar){
    dst = Mat::zeros(src.size(), src.type());

    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){
            if( src.at<uchar>(x,y) >= kstar.at(1))
                dst.at<uchar>(x,y) = 255;
            else if(src.at<uchar>(x,y) >= kstar.at(0))
                dst.at<uchar>(x,y) = 127;
        }
    }
}


int main(int argc, char** argv){

    Mat src, dst;
    src = imread(argv[1], IMREAD_GRAYSCALE);
    
    if( src.empty() )
        return -1;
    imshow("Original img", src);

    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0);

    vector<float> hist = normHistogram(blur);
    vector<int> kstar = otsu2K(hist);
    
    myThreshold(blur, dst, kstar);

    imshow("Otsu", dst);
    waitKey(0);



    return 0;
}