#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
Mat src, dst;
int ht = 100,lt = 30;

void canny(const Mat& src, Mat& dst){
    //Primo passo: Sfocare (smoothing dell'immagine con la Gaussiana)

    Mat gaussBlur;
    copyMakeBorder(src, gaussBlur, 1, 1, 1, 1, BORDER_REFLECT);
    GaussianBlur(gaussBlur, gaussBlur, Size(3,3), 0, 0);


    //Secondo passo: Calcolare la magnitudo e l'orientazione del gradiente

    //  Gradiente
    Mat sobelX, sobelY;
    Sobel(gaussBlur, sobelX, CV_32FC1, 1, 0);
    Sobel(gaussBlur, sobelY, CV_32FC1, 0, 1);

    //  Magnitudo
    Mat dX, dY, mag;
    // pow(sobelX, 2, dX);
    // pow(sobelY, 2, dY);
    // sqrt(dX+dY, mag);

    mag.convertTo(mag,CV_32FC1);
    mag = abs(sobelX) + abs(sobelY);

    // Angolo di fase
    Mat magNorm, orientation;
    normalize(mag, magNorm, 0, 255, NORM_MINMAX, CV_8UC1);
    phase(sobelX, sobelY, orientation, true);                         //true mi ritorna l'angolo in gradi
    
    
    //Terzo passo: Applicare la non maxima suppression
    Mat maxSupp;    // = Mat::zeros(magNorm.rows,magNorm.cols, magNorm.type());
    magNorm.copyTo(maxSupp);
    normalize(orientation, orientation, -180, 180, NORM_MINMAX);


    /**
     * Per ogni pixel (i,j), che è il pixel centrale che sto considerando nell’intorno, verifico se è 
     * maggiore dei suoi vicini in quella direzione (in tal caso, è un edge forte). 
     * 
     * orizzontale: i vicini sono quelli a sinistra e a destra del pixel centrale, dunque rispetto (i,j-1) e (i, j+1);
    */
    for(int i = 1; i < magNorm.rows-1; i++){
        for(int j = 1; j < magNorm.cols-1; j++){
            //orizzontale
            if(orientation.at<float>(i,j) > -22.5 && orientation.at<float>(i,j) <= 22.5 || orientation.at<float>(i,j) <= -157.5 && orientation.at<float>(i,j) > 157.5 ){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i,j + 1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i,j - 1)){
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
            //-45
            else if(orientation.at<float>(i,j) > 22.5 && orientation.at<float>(i,j) <= 67.5 || orientation.at<float>(i,j) > -157.5 && orientation.at<float>(i,j) <= -112.5 ){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1, j + 1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1,j - 1)){
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
            //+45
            else if(orientation.at<float>(i,j) > 112.5 && orientation.at<float>(i,j) <= 157.5 || orientation.at<float>(i,j) > -67.5 && orientation.at<float>(i,j) <= -22.5 ){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1, j + 1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1,j - 1)){
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
            //verticale
            else if(orientation.at<float>(i,j) > 67.5 && orientation.at<float>(i,j) <= 112.5 || orientation.at<float>(i,j) <= -67.5 && orientation.at<float>(i,j) > -112.5 ){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1,j) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1,j)){
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
        }
    }


    //Quarto passo: Applicare il thresholding con isteresi

    for(int i=1; i<maxSupp.rows-1; i++){
		for(int j=1; j<maxSupp.cols-1; j++){
			if(maxSupp.at<uchar>(i,j) > ht)
				maxSupp.at<uchar>(i,j) = 255;
            else if(maxSupp.at<uchar>(i,j) < lt) 
                maxSupp.at<uchar>(i,j) = 0;
			else if(maxSupp.at<uchar>(i,j) <= ht && maxSupp.at<uchar>(i,j) >= lt){
				bool strong_n = false;
				for(int l = -1; l <= 1 && !strong_n ; l++){
					for(int k = -1; k <= 1 && !strong_n ; k++){
						{
							if(maxSupp.at<uchar>(i+l,j+k) > ht) 
                                strong_n = true;
						}
					}
				}
				if(strong_n) 
                    maxSupp.at<uchar>(i,j) = 255;
				else 
                    maxSupp.at<uchar>(i,j) = 0;
			}
				
        }
	}

    maxSupp.copyTo(dst);

}

void CannyThreshold(int, void*){
	    canny(src,dst);

	    imshow("Canny",dst);

}



int main(int argc, char **argv){

    src = imread(argv[1], IMREAD_GRAYSCALE);
    if( src.empty() )
        return -1;

    imshow("Orginal Image", src);
    waitKey(0);
    
    namedWindow("Canny");
    createTrackbar("Trackbar th", "Canny", &ht, 255, CannyThreshold);
    createTrackbar("Trackbar lh", "Canny", &lt, 255, CannyThreshold);
    CannyThreshold(0,0);
    
    //canny(src, dst);
    //imshow("Canny", dst);
    waitKey(0);

    Mat cannyCV;
    Canny(src, cannyCV, 30, 100);
    imshow("cvCanny", cannyCV);
    waitKey(0);
    


    return 0;
}