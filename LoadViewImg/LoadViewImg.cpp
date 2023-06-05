/** Istruzioni per eseguire:
 *  g++ <nome_file>.cpp -o <nome_eseguibile> $(pkg-config --cflags --libs opencv)
 * ./<nome_eseguibile> <nome_immagine>
 *
*/

#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char** argv){

    /**
     * imread si occuperà di leggere l’immagine che passeremo in argv[1].
     *  Notare che per passare un’immagine si deve riportare il percorso dell’immagine stessa 
     * (se il percorso contiene spazi, inserirlo fra virgolette “” ). 
     * -1 demanda alla libreria come caricare l’imm.
     * restituirà un valore di tipo Mat.
     * 
     * La funzione determina automaticamente il tipo di file (BMP, JPEG, PNG…). 
     * Alloca la memoria necessaria per la struttura dati (cv::Mat) che conterrà i dati 
     * e viene deallocato automaticamente quando si esce dal contesto.
    */
    Mat img = imread(argv[1], -1);

    /**
     * Controlliamo se l’immagine è stata caricata correttamente (quindi non è vuota).
    */
    if( img.empty() )
        return -1;

    /**
     * L’immagine deve essere contenuta all’interno di un contenitore (finestra) altrimenti non
     * saremmo in grado di visualizzarla, dunque creiamo anche una finestra attraverso 
     * namedWindow(name, size), alla quale passeremo 
     * 1.nome delle finestra e 
     * 2.dimensione.
    */
    namedWindow("Example1", WINDOW_AUTOSIZE);

    /**
     * Per mostrare l’immagine usiamo imshow(nWindow, nImg), 
     * passando il 1.nome della finestra 
     * 2. immagine che vogliamo visualizzare.
    */
    imshow("Example1", img);

    /**
     * Usiamo waitKey() per visualizzare l’output, altrimenti aperta la finestra andremmo a chiuderla subito dopo. 
     * Il focus dove dovremo premere il testo è sulla finestra stessa.
    */
    waitKey(0);
    destroyWindow("Example1");

    return 0;
}