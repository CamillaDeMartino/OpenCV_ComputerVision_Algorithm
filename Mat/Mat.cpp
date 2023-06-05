#include <opencv2/opencv.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv ){
    if( argc < 2){
        cout<<"usage: "<<argv[0]<<" image_name"<<endl;
        exit(0);
    }

    //classe template Vec<>
    //int è il tipo 3 è la dimensione
    Vec<int,3> v3iT;
    for(int i=0;i<3;i++)
        v3iT[i]=i;
    for(int i=0;i<3;i++)
        cout<<"v3iT["<<i<<"]="<<v3iT[i]<<endl;

    //Alias classe template Vec<>
    //Vect3i = vettore di dimesione 3 di tipo intero (i)
    Vec3i v3i;
    for(int i=0;i<3;i++)
        v3i[i]=i;
    for(int i=0;i<3;i++)
        cout<<"v3i["<<i<<"]="<<v3i[i]<<endl;

    //classe template Matx<>. possiamo utilizzare anche dimensioni > 6 e tipi oltre a F e D
    Matx<int,7,7> m33iT;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            m33iT(i,j)=i+j;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            cout<<"m33iT["<<i<<"]["<<j<<"]="<<m33iT(i,j)<<endl;
    
    //Alias classe template Matx<>: solo dimensioni <= 6 e tipi F e D
    Matx33f m33f;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            m33f(i,j)=i+j;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            cout<<"m33f["<<i<<"]["<<j<<"]="<<m33f(i,j)<<endl;

    //Mat
    Mat image = imread(argv[1], IMREAD_GRAYSCALE);
    
    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    cout<<"Rows: "<<image.rows<<" Cols: "<<image.cols<<endl;
    cout<<"Dims: "<<image.dims<<endl;
    
    for(int i=0;i<image.dims;i++)
        cout<<"Step["<<i<<"]="<<image.step[i]<<endl;

    //È possibile creare un array semplicimente instanziando una varaibile di tipo cv:MAT
    //In questo caso l'array non ha né tipo né dimensione
    Mat image1(image.rows,image.cols,CV_8UC1,Scalar(0));
    //oppure
    Mat image2;
    image2.create(image.rows,image.cols,CV_8UC1);
    //oppure con Size()
    Mat image3(Size(image.rows,image.cols),CV_8UC1,Scalar(0));
    //oppure con inizializzazione dall'array data (della prima immagine caricata)
    Mat image4(image.rows,image.cols,CV_8UC1,image.data);
    //oppure multidimensionali
    int ndim[]={6,5,4};
    Mat image5(3,ndim,CV_32FC3,Scalar(255.0,255.0,255.0));

    //costruttori di copia
    Mat image6(image);
    //oppure solo una parte dell'immagine con Range
    Mat image7(image,Range(1,image.rows/2),Range(1,image.cols/2));
    imshow( "Display window", image7 );
    waitKey(0);
    //oppure solo una parte dell'immagine con Rect
    Mat image8(image,Rect(image.rows/4,image.cols/4,image.rows/2,image.cols/2));
    imshow( "Display window", image8 );
    waitKey(0);

    //accesso ai pixel
    for(int i=0;i<image.rows;i++)
        for(int j=0;j<image.cols;j++)
            if(i%2 && j%2)
                image.at<unsigned char>(i,j)=255-image.at<unsigned char>(i,j);
            else
                image.at<unsigned char>(Point(i,j))=255;
    imshow( "Display window", image );
    waitKey(0);

    //accesso riga
    for(int i=0;i<image.cols;i++)
        image.ptr<unsigned char>(20)[i]=0;
    imshow( "Display window", image );
    waitKey(0);

    //accesso riga e colonna: row() e col() restituisco un Mat 
    for(int i=0;i<image.cols;i++)
        image.row(40).at<unsigned char>(i)=0;
    for(int i=0;i<image.rows;i++)
        image.col(40).at<unsigned char>(i)=0;
    imshow( "Display window", image );
    waitKey(0);

    //accesso ad un blocco
    Mat block1=image.rowRange(10,200);
    //Mat block2=image.rowRange(Range(10,200));
    imshow( "Display window", block1 );
    waitKey(0);

    //Mat block3=image(Range(10,300),Range(10,400));
    Mat block4=image(Rect(10,10,200,300));
    imshow( "Display window", block4 );
    waitKey(0);

    //Utility
    Mat imageC=image.clone();
    imshow( "Display window", imageC);
    waitKey(0);

    //copia con maschera
    Mat Mask(image.rows,image.cols,CV_8UC1,Scalar(1));
    for(int i=image.rows/4;i<image.rows/2;i++)
        for(int j=image.cols/4;j<image.cols/2;j++)
            Mask.at<unsigned char>(i,j)=0;

    Mat imageCp;
    image.copyTo(imageCp);
    imageCp.setTo(0,Mask);
    imshow( "Display window", imageCp);
    waitKey(0);

    //aggiunta righe all'immagine
    image.push_back(image.rowRange(10,200));
    imshow( "Display window", image);
    waitKey(0);

    return 0;
}