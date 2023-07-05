#include <iostream>
#include <stdlib.h>
#include <stack>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


/**
 * PointShift2D definisce l’8-intorno. Serviverà per analizzare i pixel adiacenti.
 * sono i punti che definiscono gli spostamenti rispetto al pixel centrale
 * 
 *     -1   0   1
 *  -1 [ ] [ ] [ ]
 *   0 [ ] [ ] [ ]
 *   1 [ ] [ ] [ ]
*/


const Point PointShift2D[8] = 
{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1),                Point(0, 1),
    Point(1, -1),  Point(1, 0),  Point(1, 1)
};

/**
 * La fase di accrescimento 
 * utilizza lo stack: conterrà tutti i pixel che verranno man mano scoperti e inseriti nella regione.
 * All’inizio avrò solo il seed, dunque lo inserisco nello stack.
 * 
 * Finché lo stack non è vuoto, prelevo il pixel in cima allo stack e lo chiamo center. 
 * Imposto questo pixel ad 1 all’interno della maschera (etichetta). Poi rimuovo il pixel dallo stack.
 * 
 * Poi analizzo l’8-intorno di center: 
 * aggiungo al pixel center un certo scostamento in una delle 8 direzioni (dunque controllo i vicini).
 * 
 * Poi controllo innanzitutto che il pixel stimato sia interno all’immagine, altrimenti  passo direttamente alla successiva iterazione.
 * Se il pixel stimato è all’intero dell’immagine, mi calcolo la distanza euclidea (differenza
 * delle componenti al quadrato) fra il seed e il pixel che sto stimando.
 * 
 * Poi verifico alcune condizioni: 
 *  - se dest(punto_stimato) è 0, perché significa che non è ancora stato etichettato;
 *  - se mask è 0, perché significa che non è stato ancora inserito all’interno della regione;
 *  - se la distanza è inferiore alla soglia prefissata (delta<threshold).
 * Se queste 3 condizioni sono rispettati allora inserisco il pixel all’interno della regione e poi faccio il push sullo stack in modo che 
 * in una delle prossime iterazioni considereremo anche l’8 intorno di questo pixel per accrescere la regione.
*/
void grow(const Mat& src, const Mat& dst, Mat& mask, Point seed, int th){
    
    stack<Point> pointStack;
    pointStack.push(seed);

    while( !pointStack.empty() ){
        //estraggo il pixel dallo stack
        Point center = pointStack.top();
        //imposto l'etichetta a 1
        mask.at<uchar>(center) = 1;
        pointStack.pop();

        for(int i = 0; i < 8; i++){
            //sommo gli spostamenti
            Point estimatingPoint = center + PointShift2D[i];

            //se il pixel non è all'interno dell'immagine valuto il prossimo
            if( estimatingPoint.x < 0 ||
                estimatingPoint.y < 0 ||
                estimatingPoint.x > src.cols-1 ||
                estimatingPoint.y > src.rows-1 ){
                    
                    continue;
                }
                //se è all'interno dell'immagine mi caloclo la distanza euclidea
                //n.b delta, ossia la distanza è quello che ho scelto come predicato 
                // è un'immagine RGB quindi faccio la somma della “differenza delle 3 componenti al quadrato”.
            else{

                int delta = int(  pow(src.at<Vec3b>(seed)[0] - src.at<Vec3b>(estimatingPoint)[0], 2) 
                                + pow(src.at<Vec3b>(seed)[1] - src.at<Vec3b>(estimatingPoint)[1], 2) 
                                + pow(src.at<Vec3b>(seed)[2] - src.at<Vec3b>(estimatingPoint)[2], 2));
                
                if( dst.at<uchar>(estimatingPoint) == 0 &&
                    mask.at<uchar>(estimatingPoint) == 0 &&
                    delta < th
                    ){
                        mask.at<uchar>(estimatingPoint) = 1;
                        pointStack.push(estimatingPoint);
                    }
            }
        }
    }
}

/**
 * Mask è un’immagine di appoggio che usiamo per segnarci tutti i pixel che appartengono ad una data regione. 
 * Man mano che l’algoritmo procede, si parte da un seed (che nella maschera verrà messo ad 1)
 *  e man mano che i pixel vengono aggiunti a questa regione verranno impostate le posizioni corrispondenti ad 1 all’interno della maschera.
 * 
 * In questo modo, alla fine di un’iterazione di accrescimento all’interno della maschera avrò tutti i pixel di quella regione ad 1 
 * (e dunque potrò farò diverse operazioni, come assegnargli un’etichetta o estrarre quella porzione dall’immagine originale).
 * 
 * Consideriamo i pixel di tutta l’immagine come potenziali seed, e considero come primo seed il primo pixel in dst che è ancora 0. 
 * A partire dal pixel (x,y) effettuo la fase di accrescimento attraverso la funzione grow().
 * 
 * Al termine della funzione, mask avrà tutti i pixel che appartengono alla regione.
 * Poi calcolo l’area occupata da questa regione facendo una somma di tutti i pixel ad 1 dell’immagine. 
 * Se questa regione è abbastanza grande rispetto all’area minima, allora vado a sommare in dst la mask più il padding. 
 * Altrimenti imposto in dst l’etichetta a 255 (che sarebbe la regione don’t care).
 * 
 * Il padding è l'etichetta che assegnamo alla regione. Questo parte da 1 e lo incrementiamo per ogni nuova regione (dunque la prima regione sarà 1, la seconda sarà 2, ecc.). 
 * Posso anche definire un numero massimo di regioni, per cui se supero tale limite significa che sto over-segmentando.
 * Poi riporto la maschera a 0 così da prepararla per il prossimo seed.
*/

void regionGrowing(const Mat& src, Mat& dst, double minAreaFactor,int maxRegionNum, int th){

    //se troviamo una regione troppo piccola questa viene scartata (cioè posta a 255)
    int min_region_area = int(minAreaFactor * src.cols * src.rows);
    //matrice per i seed, inizializzo a 0 -> tutti possibili seed
    dst = Mat::zeros(src.size(), CV_8UC1); 
    //maschera che uso per definire l'appartenenza a una regione (1 appartiene 0 no)
    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    //etichetta che assegno alla regione
    uchar label = 1;

    for(int x = 0; x < src.cols; x++){
        for(int y = 0; y < src.rows; y++){
            //se il pixel non è stato inserito in nessuna regione è un possibile seed -> accresco
            if(dst.at<uchar>(Point(x,y)) == 0){

                grow(src, dst, mask, Point(x,y), th);
                int maskArea = (int)sum(mask).val[0];

                if( maskArea > min_region_area ){
                    dst = dst + mask * label;

                    imshow("Mask", mask*255);
                    waitKey(0);

                    if(++label > maxRegionNum ){
                        printf("Out of Region");
                        exit;
                    }               
                }
                else{
                    //troppo piccola
                    dst = dst + mask * 255;
                }
                //azzera la maschera
                mask = mask - mask;
            }
        }
    }
} 

int main(int argc, char** argv){
    
    Mat src, dst;
    src = imread(argv[1]);

    if( src.empty() )
        return -1;

    if (src.cols > 500 || src.rows > 500) {
        resize(src, src, Size(0, 0), 0.5, 0.5); // resize for speed 
    }

    imshow("Original Img", src);
    waitKey(0);
    const int th = 150;
    const int maxRegionNum = 10;
    const double minAreaFactor = 0.01;

    regionGrowing(src, dst, minAreaFactor, maxRegionNum, th);

    imshow("RegionGr", dst);
    waitKey(0);

    return 0;
}