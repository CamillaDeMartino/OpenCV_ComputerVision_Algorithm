#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/**
 * Utilizziamo laplaciano per calcolare un'immagine di edge
 * Applicare la soglia th all'immagine di edge ottenendo un'immagine binaria
*/
void edgeDetection(const Mat& src, Mat &dst, int th){

    Mat lapImg, edge;
    Laplacian(src, lapImg, src.type());

    convertScaleAbs(lapImg, edge);
    threshold(edge, dst, th, 255, THRESH_BINARY);

}


/**
 * Individuiamo un valore di soglia, serve per individuare i pixel di edge
 * 
 * Otteniamo così un'immagine binaria
*/
int pixelEdge(const Mat& edgeImg, vector<Point>& pixelEdges){

    int numPixelEdge = 0;

    for(int x = 0; x < edgeImg.rows; x++){
        for(int y = 0; y < edgeImg.cols; y++){
            if( edgeImg.at<uchar>(x,y) == 255 ){
                numPixelEdge++;
                Point pixel;
                pixel.x = x;
                pixel.y = y;
                pixelEdges.push_back(pixel);
            }
        }
    }

    return numPixelEdge;
}


/**
 * L'istogramma normalizzato questa volta viene calcolato in baste ai pixel di edge calcolati
 * 
 * Quindi dobbiamo calcolare l'istogramma utilizzando solo i pixel dell'immagine di input che
 * corrispondono alle posizioni dei pixel con valore 1 nell'immaginme binaria
*/
vector<float> normHistogram(const Mat& src, const vector<Point>& pixelEdges, int numPixelEdge){

    vector<float> hist(256, 0.0f);      //è vettore che rappresenta l'istogramma, è composto fa 256 elementi float inizializzati a 0
    for(const auto& i : pixelEdges){
            hist.at(src.at<uchar>(i.x, i.y))++;
        
    }

    for(int i = 0; i < 256; i++){
        hist.at(i) = numPixelEdge > 0 ? hist.at(i)/numPixelEdge : 0; 
    }

    return hist;
}

vector<float> probability(vector<float> hist){

    vector<float> prob(256, 0.0f);

    prob.at(0) = hist.at(0);

    for(int i = 1; i < 256; i++){
        prob.at(i) = prob.at(i-1) + hist.at(i);
    }

    return prob;
}


vector<float> cumulativeAvg(vector<float> hist){

    vector<float> cumAvg(256, 0.0f);

    cumAvg.at(0) = hist.at(0);

    for(int i = 1; i < 256; i++){
        cumAvg.at(i) = cumAvg.at(i-1) + i * hist.at(i);
    }

    return cumAvg;
}

float globalAvg(vector<float> hist){

    float globalAvg = 0.0f;

    for(int i = 0; i < 256; i++){
        globalAvg += i*hist.at(i);
    }

    return globalAvg;
}

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

int otsuEdge(const Mat& src, int th){

    Mat blur, edge;
    vector<Point> pixel;
    GaussianBlur(src, blur, Size(3,3), 0);

    edgeDetection(blur, edge, th);
    int numPixelEdge = pixelEdge(edge, pixel);

    vector<float> normHist = normHistogram(blur, pixel, numPixelEdge);
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

    imshow("Original Image", src);

    threshold(src, dst, otsuEdge(src, 5), 255, THRESH_BINARY);
    imshow("Otsu Edge", dst);
    waitKey(0);

    return 0;
}