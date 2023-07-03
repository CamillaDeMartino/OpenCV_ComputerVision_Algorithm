#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

using namespace std;
using namespace cv;

/**
 * Istogramma Normalizzato
 * 
 * Un istogramma normalizzato consiste nel calcolare il numero dei pixel che assumono 
 * un particolare livello di intensità e dividerlo per il numero totale di pixel (numRighe*numColonne).
 * 
 * Scorriamo con un doppio ciclo for l'immagine, e consideriamo ad ogni iterazione un pixel 
 * che avrà un valore di intensità compreso fra 0 e 255.
 * Utilizziamo questo valore come indice del vettore che rappresenta l'istogramma, così che se è già presente
 * un pixel in quella posizione, significa che ci sono più pixel con la stessa intensità, incrementiamo dunque 
 * quella cella.
 * 
 * Es. se nell’immagine ho 200 pixel la cui intensità è 50, in his[50] avrò valore 200.
 * 
 * Poi dividiamo ogni posizione dell'istogramma per il numeor di pixel.
*/

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
 * Calcoliamo le somme cumulative, che ricordiamo essere “la probabilità che un pixel appartenga ad una certa classe”.
 * Dunque: al variare della soglia calcoliamo la probabilità che un pixel appartenga alla prima classe.
 * 
 * Essendo somme cumulative, la somma al passo i-simo sarà data considerando la somma al passo precedente
*/

vector<float> probability(vector<float> hist){

    vector<float> prob(256, 0.0f);

    prob.at(0) = hist.at(0);

    for(int i = 1; i < 256; i++){
        prob.at(i) = prob.at(i-1) + hist.at(i);
    }

    return prob;
}


/**
 * Calcoliamo la media cumulativa.
 * 
 * Per ogni soglia i, hist[i] è  moltiplicata per i (ovvero la soglia corrente).
 * Essendo anche in questo caso cumulative, la somma al passo i-simo sarà data considerando la somma al passo precedente.
 * 
 * (nb: obv se i=0, i*his[0] = 0, dunque non abbiamo bisogno di calcolare cumAvg[0]).
*/

vector<float> cumulativeAvg(vector<float> hist){

    vector<float> cumAvg(256, 0.0f);

    cumAvg.at(0) = hist.at(0);

    for(int i = 1; i < 256; i++){
        cumAvg.at(i) = cumAvg.at(i-1) + i * hist.at(i);
    }

    return cumAvg;
}


/**
 * Calcoliamo la media globale. 
 * 
 * In questo caso si tratta ovviamente di un unico valore.
 */

float globalAvg(vector<float> hist){

    float globalAvg = hist.at(0);

    for(int i = 1; i < 256; i++){
        globalAvg += i*hist.at(i);
    }

    return globalAvg;
}


/**
 * Calcoliamo la varianza interclasse  σ2B(k). 
 * 
 * Consideriamo un vettore nel quale calcoliamo la varianza interclasse per ogni soglia i-sima.
 * 
 * N.B.: in questo caso possono verificarsi delle divisioni per 0.
 * può presentarsi perché verso la fine prob1 avrà un valore così vicino ad 1 
 * che potrebbe essere direttamente arrotondato ad 1, e dunque quando facciamo
 * (prob1*(1-prob1)) sarebbe (1*(1-1)) = 0. Dunque divideremo per 0.
*/


vector<float> interVariance(float globAvg, vector<float> cumAvg, vector<float> prob){

    vector<float> sigma(256, 0.0f);
    float num, denom;

    for(int i = 0; i < 256; i++){
        num = pow(((globAvg * prob.at(i)) - cumAvg.at(i)), 2);
        denom = prob.at(i)*(1 - prob.at(i));

        sigma.at(i) = denom == 0 ? 0 : num/denom;
    }

    return sigma;
}


/**
 * Otteniamo k*, la soglia ottimale per cui σ2B(k) è massimo, calcolando la posizione del massimo del vettore sigma.
*/

int kStar(vector<float> sigma){

    //auto maxVariance = max_element(sigma.begin(), sigma.end());  //trova l'indice dell'elemento massimo 
    //int k = distance(sigma.begin(), maxVariance);
    float maxVariance = sigma.at(0);
    int k = 0;

    for(int i = 1; i < sigma.size(); i++){
        if(sigma.at(i) > maxVariance){
            maxVariance = sigma.at(i);
            k = i;
        }
    }

    return k;

}

/**
 * Prima di calcolare otsu è meglio fare un blur dell'immagine per camcellare rumore
*/
int otsu(const Mat& src){

    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0);

    vector<float> normHist = normHistogram(blur);
    vector<float> prob = probability(normHist);
    vector<float> cumAvg = cumulativeAvg(normHist);
    float globAvg = globalAvg(normHist);
    vector<float> sigma = interVariance(globAvg, cumAvg, prob);
    int k = kStar(sigma);


    return k;  

}




int main(int argc, char** argv){

    Mat src, dst;

    src = imread(argv[1], IMREAD_GRAYSCALE);
    if( src.empty() )
        return -1;

    imshow("Orginal Img", src);

    threshold(src, dst, otsu(src), 255, THRESH_BINARY);
    
    imshow("Otsu", dst);
    waitKey(0);


    return 0;
}