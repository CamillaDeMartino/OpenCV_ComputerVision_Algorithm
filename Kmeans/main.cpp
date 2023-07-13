/**
 * L’algoritmo k-means consiste nei seguenti passi fondamentali:
 * 1. inizializzare i centri di ogni cluster;
 * 2. assegnare ogni pixel al cluster con il centro più vicino.
 *    Per ogni pixel pj calcolare la distanza (euclidea) dai k centri ci, ed assegnare pj al cluster con il centro ci più vicino;
 * 3. aggiornare i centri. Dunque calcolare la media dei pixel in ogni cluster.
 * 
 * Ripetere i punti 2 e 3 finché il centro (media) di ogni cluster non viene più modificata (o comunque è minima).
 * 4. Assegnare ad ogni pixel del cluster i-simo il valore di c[i].
*/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;

/**
 * Questa funzione inizializza i centroidi dei cluster selezionando casualmente i pixel dell'immagine 
 * e li salva insieme alle informazioni sui punti associati a ciascun centro del cluster. 
 * Questi dati saranno utilizzati successivamente nell'algoritmo K-means per assegnare i punti ai cluster corrispondenti.
 * 
 * RNG random(getTickCount()); 
 * Questa riga inizializza il generatore di numeri casuali utilizzando il valore corrente di tick count come seme. 
 * 
 * Per ogni cluster inizializziamo le coordinate del centroide in modo casuale all'interno dei limiti dell'immagine
 * 
 * Calcoliamo il valore colore del pixel calcolato in modo casuale dall'immagine. 
 * Il valore colore ottenuto viene utilizzato per creare un oggetto Scalar (centerK), che rappresenta il centro del cluster
 * questo centro viene aggiunnti al vettore che conterrà i centroidi di tutti i cluster
 * 
 * Viene creato un vettore per memorizzare i futuri pixel associati a questo centro del cluster
 * Questo vettore viene aggiunti ad un altro vettore che conerrà i vettori di punti associati a ciascun cluster
 * 
*/
void createClusterInfo(const Mat& src, int clusters_number, vector<Scalar> &clusterCentres, vector<vector<Point>> &ptInClusters){

    //Inizializzazione del generatore di numeri casuali
    RNG random(getTickCount());

    for(int k = 0; k < clusters_number; k++){
        //Pixel casuale nell'immagine per inizializzare il centro del cluster
        Point centerKPoint;
        centerKPoint.x = random.uniform(0, src.cols);
        centerKPoint.y = random.uniform(0, src.rows);

        //Calcoliamo il valore colore del centro 
        Scalar centerPixel = src.at<Vec3b>(centerKPoint.y, centerKPoint.x);
        Scalar centerK(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
        clusterCentres.push_back(centerK);

        //Creiamo un vettore per memorizzare i futuri pixel associati al centro
        vector<Point> ptInClusterK;
        ptInClusters.push_back(ptInClusterK);

    }
}

/**
 * Calcoliamo la distanza tra due pixel in base ai loro valori di colore.
 * La differenza tra i canali dei colori viene calcolata per i canali blu, verde e rosso, 
 * poi utilizziamo la distanza euclidea per ottenere la distanza finale.
 * 
 * Usiamo la distanza per valutare la similarità tra i pixel.
*/
double computeColorDistance(Scalar pixel, Scalar clusterPixel){

    double diffBlue = pixel.val[0] - clusterPixel.val[0];
    double diffGreen = pixel.val[1] - clusterPixel.val[1];
    double diffRed = pixel.val[2] - clusterPixel.val[2];

    double distance = sqrt(pow(diffBlue, 2) + pow(diffGreen, 2) + pow(diffRed, 2));
    return distance;
}

/**
 * Iteriamo su tutti i pixel dell'immagine e, per ciascun pixel, troviamo il cluster più vicino 
 * calcolando la distanza colore tra tale pixel e i pixel dei cluster.
 * 
 * Successivamente il pixel viene associalo al cluster più vicino
*/
void findAssociatedCluster(const Mat& src, int clusters_number, vector<Scalar> clustersCenters, vector<vector<Point>> &ptInClusters){

    for(int r = 0; r < src.rows; r++){
        for(int c = 0; c < src.cols; c++){

            //inizializziamo la distanza minima
            double minDistance = INFINITY;
            //inizializziamo l'indice del cluster più vicino a 0
            int closestClusterIndex = 0;
            
            Scalar pixel = src.at<Vec3b>(r, c);

            for(int k = 0; k < clusters_number; k++){
                
                //pixel del cluster
                Scalar clusterPixel = clustersCenters[k];

                //distanza tra pixel e il k center
                double distance = computeColorDistance(pixel, clusterPixel);

                //aggiorna il cluster più vicino se la distanza è minore della distanza minima attuale
                if(distance < minDistance){
                    minDistance = distance;
                    closestClusterIndex = k;
                }
            }

            //salva il pixel nel cluster associati
            ptInClusters[closestClusterIndex].push_back(Point(c,r));
        }
    }
}

/**
 * Aggiorniamo i centri dei cluster in base alla media dei pixel associati a ciascun cluster, 
 * In particolare:
 *  - Per tutti i pixel di ogni cluster vengono inizializzati i canali colore a 0
 *  - Per ciascun pixel viene preso il loro valore corrispondente nell'immagine fatta la media
 *  - Viene creato un nuovo valore per il centro del cluster utilizzando i valori medi calcolati
 *  - la distanza tra il nuovo centro e il precedente viene calcolata utilizzando computeColorDistance
 *  - dividiamo il nuovo valore per il numero totale di cluster per ottenere il nuovo vaore dei centro medio
 *  - 
*/
double adjustClusterCenters(const Mat& src, int clusters_number, vector<Scalar> &clustersCentres, vector<vector<Point>> ptInClusters, double &oldCenter, double newCenter ){
    
    double diffChange;

    for(int k = 0; k < clusters_number; k++){
        //prendo i pixel in un cluster k
        vector<Point> ptInCluster = ptInClusters[k];
        double newBlue = 0;
        double newGreen = 0;
        double newRed = 0;

        //Calcola i valori medi per i 3 canali colore
        for(int i = 0; i < ptInCluster.size(); i++){
            //ottengo il valore del pixel nell'immagine
            Scalar pixel = src.at<Vec3b>(ptInCluster[i].y, ptInCluster[i].x);
            newBlue += pixel.val[0];
            newGreen += pixel.val[1];
            newRed += pixel.val[2];
        }
        newBlue /= ptInCluster.size();
        newGreen /= ptInCluster.size();
        newRed /= ptInCluster.size();

        //Assegna il nuovo valore di colore al centro del cluster
        Scalar newPixel(newBlue, newGreen, newRed);

        //Calcola la distanza tra i vecchi e i nuovi valori 
        newCenter += computeColorDistance(newPixel, clustersCentres[k]);
        clustersCentres[k] = newPixel;
    }

    //calcola il nuovo valore del centro medio
    newCenter /= clusters_number;

    // Calcola la differenza tra il cambiamento nella precedente iterazione
    diffChange = abs(oldCenter - newCenter);
    cout<<"DiffChange is: "<< diffChange << endl;
    oldCenter = newCenter;

    return diffChange;
}

/**
 * Applichiamo i colori finali ai pixel dell'immagine di output in base ai cluster associati
*/

void applyFinalClusterToImage(Mat& dst, int clusters_number, vector<vector<Point>> ptInClusters, vector<Scalar> &clustersCenters ){
    
    //srand(time(NULL));

    //assegna un colore casuale a ciascun cluster
    for(int k = 0; k < clusters_number; k++){

        //Ottiene i punti associati a questo cluster
        vector<Point> ptInCluster = ptInClusters[k];

        //Per ogni pixel nel cluster cambia il colore per adattarlo al cluster
        for(int i = 0; i < ptInCluster.size(); i++){

            //Scalar pixelColor = dst.at<Vec3b>(ptInCluster[i]);
            //Assegna il colore del centro del cluster come nuovo colore del pixel
            Scalar pixelColor = clustersCenters[k];

            //Aggiorna il colore del pixel nell'immagine di output
            dst.at<Vec3b>(ptInCluster[i])[0] = pixelColor.val[0];
            dst.at<Vec3b>(ptInCluster[i])[1] = pixelColor.val[1];
            dst.at<Vec3b>(ptInCluster[i])[2] = pixelColor.val[2];
        }
    }

}



int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_COLOR);

    if(src.empty())
        return -1;

    imshow("Original img", src);

    int clusters_number = stoi(argv[2]);

    vector<Scalar> clustersCenters;
    vector< vector<Point>> ptInClusters;
    double threshold = 0.1; //per stoppare le iterazioni
    double oldCenter = INFINITY;
    double newCenter = 0;
    double diffChange = oldCenter - newCenter;

    //crea random centri dei cluster e i vecctori cluster
    createClusterInfo(src, clusters_number, clustersCenters, ptInClusters);

    //itera finché i centri dei cluster sono distanti pari alla soglia
    while( diffChange > threshold ){

        //resetta i cambiamenti
        newCenter = 0;

        //pulisci i pixel associati ad ogni cluster
        for(int k = 0; k < clusters_number; k++){
            ptInClusters[k].clear();
        }

        //cerca i pixel più vicini ai centri dei cluster
        findAssociatedCluster(src, clusters_number, clustersCenters, ptInClusters);

        //ricalcola i centri dei cluster
        diffChange = adjustClusterCenters(src, clusters_number, clustersCenters, ptInClusters, oldCenter, newCenter);

    }

    Mat imgOutputKNN = src.clone();
    applyFinalClusterToImage(imgOutputKNN, clusters_number, ptInClusters, clustersCenters);
    imshow("Segmentation", imgOutputKNN);
    
    waitKey(0);

}