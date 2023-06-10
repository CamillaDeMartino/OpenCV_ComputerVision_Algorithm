/**
 * L’algoritmo di Split and Merge può essere nei seguenti passi
 *  1. dividere in quattro quadranti tutte le regioni per cui il predicato Q risulta falso;
 *  2. dopo la fase di split, si applica il processo di merging a tutte le regioni adiacenti R i e Rj, per cui Q(Ri U Rj) = Vero;
 *  3. il processo termina quando non è più possibile effettuare unioni.
 * 
 * Solitamente si definisce una dimensione minima della regione oltre la quale non si effettua lo split
 * Per ragioni di efficienza, la fase di merge si può eseguire se il predicato è vero per le 
 * singole regioni adiacenti (non si effettua l’unione).
*/


#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

double smthreshold; 
int tsize;
int media;

/**
 * La classe TNode rappresenta un nodo del Quadtree.
 * L’oggetto Rect region rappresenta la regione dell’immagine. 
 * I 4 figli del nodo sono UL, UR, LL, LR (Upper Left, Upper Right, Lower Left e Lower Right).
 * 
 * Ho una vettore di puntatori a Tnode merged che rappresenta le regioni che devono essere unite (queste regioni saranno i figli).
 * 
 * MergedB è un vector di bool che mi dice se due regioni figlie sono state unite.
 * 
 * Poi ho il costruttore (imposto la regione e metto a null i puntatori a nodi).
 * 
 * Per determinare il predicato dovrò calcolare la deviazione standard stddev e la media della regione. 
 * In particolare lavoro sulla deviazione standard mentre userò la media per assegnare un’etichetta alla regione finale.
 * 
 * Per aggiungere una regione userò addRegion.
 * Il resto sono semplicemente getter e setter.
*/
class TNode{
    private:
        Rect region;
        TNode *UL, *UR, *LL, *LR;
        vector<TNode *> merged;
        vector<bool> mergedB = vector<bool>(4, false);
        double stddev, mean;
    public:
        TNode(Rect R){
            region = R;
            UL = UR = LL = LR = nullptr;
        };

        TNode *getUL(){return UL;};
        TNode *getUR(){return UR;};
        TNode *getLL(){return LL;};
        TNode *getLR(){return LR;};

        void setUL(TNode *N){UL = N;};
        void setUR(TNode *N){UR = N;};
        void setLL(TNode *N){LL = N;};
        void setLR(TNode *N){LR = N;};

        double getStdDev(){return stddev;};
        double getMean(){return mean;};

        void setStdDev(double stddev){this->stddev = stddev;};
        void setMean(double mean){this->mean = mean;};

        void addRegion(TNode *R){ merged.push_back(R);};
        vector<TNode *> &getMerged(){return merged;};

        Rect &getRegion(){return region;};

        void setMergedB(int i){mergedB[i] = true;};
        bool getMergedB(int i){return mergedB[i];};
};

/**
 * Split si tratta di una funzione ricorsiva: se entro nella funzione split significa che ho una nuova regione e quindi alloco un nuovo Tnode. 
 * R è la regione dell’immagine rappresentata dal nodo. 
 * Ovviamente, la prima volta (dunque nel main) R parte da (0,0) e l’altezza e l’ampiezza dell’immagine sono 
 * il numero di righe e di colonne dell’immagine, dunque vado a prendere tutta l’immagine.
 * 
 * Poi mi calcolo media e deviazione standard(quanto i volori dei pixel distano dalla media) all’interno della regione 
 * attraverso la funzione di OpenCV meanStdDev. 
 * Una volta calcolati, li vado ad impostare negli attributi corrispondenti di root.
 * 
 * Poi prima di iniziare la fase di splitting ricorsiva, dobbiamo controllare se la regione > tsize, 
 * dunque se quella regione può essere ulteriormente splittata, e se la deviazione standard è maggiore di un determinato threshold. 
 * In tal caso, effettuo lo splitting. Avrò 4 regioni tutte che avranno la metà dell’ampiezza e dell’altezza dell’immagine originale.
 * 
 * Per ogni regione calcolata, per il nodo corrispondete dovrò richiamare ricorsivamente la funzione split 
 * a cui passerò l’immagine originale e la regione calcolata. 
 * Ricordiamo che split ritornerà la radice dell’albero quadtree, che noi assegneremo al nodo.
 * 
 * Ovviamente la fase di split terminerà quando una delle due condizioni diventa falsa, 
 * dunque se la regione considerata è troppo piccola oppure la deviazione standard è sotto la soglia 
 * (dunque ho una regione piuttosto uniforme come livelli di grigio).
 * 
 * Inoltre, visualizzo il contorno del rettangolo sull’immagine (rectangle(img,R,Scalar(0)), dunque in output avremo il Quadtree.
 * Infine, ritorno la radice.
*/

TNode *split(Mat &img, Rect R){

    TNode *root = new TNode(R);
    Scalar stddev, mean;
    meanStdDev(img(R), mean, stddev); //img(R) significa sto passando una regione dell'immagine

    root->setMean(mean[0]);
    root->setStdDev(stddev[0]);

    if(R.width > tsize && root->getStdDev() > smthreshold){
        
        Rect ul(R.x, R.y, R.height/2, R.width/2);
        root->setUL(split(img, ul));

        Rect ur(R.x, R.y + R.width/2, R.height/2, R.width/2);
        root->setUR(split(img, ur));

        Rect ll(R.x+R.height/2, R.y, R.height/2, R.width/2);
        root->setLL(split(img, ll));

        Rect lr(R.x+R.height/2,R.y+R.width/2, R.height/2, R.width/2);
        root->setLR(split(img, lr));
    }

    rectangle(img, R, Scalar(0));
    return root;

}

/**
 * Nella fase di merge, parto dalla radice e verifico, regione per regione, se posso unire regioni adiacenti. 
 * In questo caso sto considerando solo regioni adiacenti che appartengono alla stessa regione 
 * (e dunque non considero le sottoregioni che si trovano in altre regioni).
 * 
 * Prima di analizzare la regione verifico se l’ampiezza è maggiore di tsize e se la deviazione standard è maggiore della threshold: 
 * in tal caso, significa che la regione è stata splittata, e quindi ho delle sottoregioni che potrei fondere.
 * 
 * Altrimenti, significa che la regione non è stata splittata, dunque inserisco all’interno dell’array merged della regione 
 * l’intera regione stessa, in quanto deve essere considerata integralmente.
 * 
 * Come flag per indicare che tutta la regione deve essere unita imposto a true tutti i valori booleani dell’array MergedB.
 * 
 * Se la regione è stata splittata, verifico se posso fare delle unioni: il procedimento è analogo per ogni if
 *  - Nel primo if provo ad unire Upper Left e Upper Righ. 
 *      Per verificare se queste due regioni possono essere unite dobbiamo prendere in considerazione la deviazione standard
 *      stddev della regione e verificare se entrambe sono minori di una certa threshold.
 *      Se così fosse, inserisco ambo le regioni nell’array merged e imposto ambo i corrispettivi booleani di mergedB a true.
 *  - Avendo unito queste posso anche verificare se posso unire Lower Left e Lower Right. 
 *    Se non posso, richiamo ricorsivamente il merge su di loro
 * 
 _____   ____
|UL|UR| |0|1|
------  -----
|LL|LR| |3|2|
------- -----
*/

void merge(TNode *root){

    if(root->getRegion().width > tsize && root->getStdDev() > smthreshold){
        //PRIMA RIGA: UL-UR
        if(root->getUL()->getStdDev() <= smthreshold && root->getUR()->getStdDev() <= smthreshold && abs(root->getUL()->getMean() - root->getUR()->getMean()) < media){
            root->addRegion(root->getUL());
            root->addRegion(root->getUR());
            root->setMergedB(0);
            root->setMergedB(1);
            if(root->getLL()->getStdDev() <= smthreshold && root->getLR()->getStdDev() <= smthreshold && abs(root->getLL()->getMean() - root->getLR()->getMean()) < media){
                root->addRegion(root->getLL());
                root->addRegion(root->getLR());
                root->setMergedB(2);
                root->setMergedB(3);
            }
            else{
                merge(root->getLL());
                merge(root->getLR());
            }
        }
        //SECONDA COLONNA: UR-LR
        else if(root->getUR()->getStdDev() <= smthreshold && root->getLR()->getStdDev() <= smthreshold && abs(root->getUR()->getMean() - root->getLR()->getMean()) < media){
            root->addRegion(root->getUR());
            root->addRegion(root->getLR());
            root->setMergedB(1);
            root->setMergedB(2);
            if(root->getUL()->getStdDev() <= smthreshold && root->getLL()->getStdDev() <= smthreshold && abs(root->getUL()->getMean() - root->getLL()->getMean()) < media){
                root->addRegion(root->getUL());
                root->addRegion(root->getLL());
                root->setMergedB(0);
                root->setMergedB(3);
            }
            else{
                merge(root->getUL());
                merge(root->getLL());
            }
        }
        //SECONDA RIGA: LR-LL
        else if(root->getLR()->getStdDev() <= smthreshold && root->getLL()->getStdDev() <= smthreshold && abs(root->getLR()->getMean() - root->getLL()->getMean()) < media){
            root->addRegion(root->getLR());
            root->addRegion(root->getLL());
            root->setMergedB(3);
            root->setMergedB(2);
            if(root->getUL()->getStdDev() <= smthreshold && root->getUR()->getStdDev() <= smthreshold && abs(root->getUR()->getMean() - root->getUL()->getMean()) < media){
                root->addRegion(root->getUR());
                root->addRegion(root->getUL());
                root->setMergedB(0);
                root->setMergedB(1);
            }
            else{
                merge(root->getUL());
                merge(root->getUR());
            }
        }
        //PRIMA COLONNA: UL-LL
        else if(root->getUL()->getStdDev() <= smthreshold && root->getLL()->getStdDev() <= smthreshold && abs(root->getUL()->getMean() - root->getLL()->getMean()) < media){
            root->addRegion(root->getUL());
            root->addRegion(root->getLL());
            root->setMergedB(0);
            root->setMergedB(3);
            if(root->getUR()->getStdDev() <= smthreshold && root->getLR()->getStdDev() <= smthreshold && abs(root->getUR()->getMean() - root->getLR()->getMean()) < media){
                root->addRegion(root->getUR());
                root->addRegion(root->getLR());
                root->setMergedB(1);
                root->setMergedB(2);
            }
            else{
                merge(root->getUR());
                merge(root->getLR());
            }
        }
        //merge tutti e 4 nessuno è una regiona
        else{
            merge(root->getUL());
            merge(root->getUR());
            merge(root->getLR());
            merge(root->getLL());
        }
    }
    else{
        root->addRegion(root);
        root->setMergedB(0);
        root->setMergedB(1);
        root->setMergedB(2);
        root->setMergedB(3);
    }

}

/**
 * La segmentazione inserisce dei valori fra le regioni che sono state unite. 
 * All’inizio recupererò l’array merged.
 * Se l’array non contiene nulla significa che quella regione è stata splittata in 4, e dunque richiamo segment sulle 4 sottoregioni.
 * Altrimenti, nell’array merged potrò avere 1 valore o >1 valori.
 * All’interno vado a metterci la media dei pixel contenuti all’interno della regione, o per motivi di efficienza faccio direttamente la “media delle medie”.
 * Poi assegno questa approssimazione del valore medio val in tutte le regioni che sono state inserite in tmp.
 *  - Se è 1, significa che ho inserito tutta la regione. In pratica si tratta di una foglia del nostro quadtree. Non faccio nessun altra operazione.
 *  - Se è >1, significa che ho inserito più di una regione. 
 *    Andrò dunque a verificare quali di queste non sono state unite, cioè vado a vedere in ogni posizione di mergedB
 *    (con cui mi segno proprio quali regioni ho unito) se il valore è false, 
 *    perché in tal caso significa che la regione non è stata segmentata e dunque richiama ricorsivamente segment su quella regione.
*/

void segment(TNode *root, Mat &img){

    vector<TNode *> tmp = root->getMerged();

    if( !tmp.size() ){
        segment(root->getUL(), img);
        segment(root->getUR(), img);
        segment(root->getLL(), img);
        segment(root->getLR(), img);
    }
    else{
        double val = 0;
        for(auto x:tmp){
            val += (int)x->getMean();
        }
        val /= tmp.size();

        for(auto x:tmp){
            img(x->getRegion()) = (int)val;
        }

        if(tmp.size() > 1){
            if(!root->getMergedB(0))
                segment(root->getUL(), img);
            if(!root->getMergedB(1))
                segment(root->getUR(), img);
            if(!root->getMergedB(2))
                segment(root->getLR(), img);
            if(!root->getMergedB(3))
                segment(root->getLL(), img);
        }
    }
}



int main(int argc, char** argv){

    Mat src = imread( argv[1], IMREAD_GRAYSCALE );
    if( src.empty())
        exit(-1);
    
    smthreshold = stod(argv[2]);
    tsize = stoi(argv[3]);
    media = stoi(argv[4]);
    
    GaussianBlur(src, src, Size(5,5), 0, 0);

    int exponent = log(min(src.cols, src.rows)) / log (2);
    int s = pow(2.0, (double)exponent);
    Rect square = Rect(0,0, s,s);
    src = src(square).clone();
    Mat srcMer=src.clone();
    Mat srcSeg=src.clone();

    TNode *root = split(src, Rect(0,0, src.rows, src.cols));
    merge(root);
    segment(root, srcSeg);

    imshow("Quad Tree", src);
    imshow("Segmented", srcSeg);
    waitKey(0);

    return 0;
}