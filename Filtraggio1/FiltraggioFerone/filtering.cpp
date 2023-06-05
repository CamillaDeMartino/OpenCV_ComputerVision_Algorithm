#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace cv;

int main( int argc, char** argv ){

	int dim,th;
	
	if( argc < 2){
        cout<<"usage: "<<argv[0]<<" image_name"<<endl;
        exit(0);
    }
    String imageName = argv[1];

	//Lettura immagine    
    Mat image;
    image = imread( imageName , IMREAD_GRAYSCALE );
    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    //Lettura dimensione filtro
    cout<<"Inserire dimensione filtro: ";
    cin>>dim;

    //Allocazione filtro e convoluzione
    Mat average_filter = Mat::ones(dim,dim,CV_32F)/(float)(dim*dim);
    
    //Per la convoluzione ruotare il filtro di 180°
    //rotate(filter,r_filter,ROTATE_180);
    
    Mat output1;
    filter2D(image,output1,image.type(),average_filter);

    //Blurring average filter
    Mat output2;
    blur(image,output2,Size(dim,dim));

    //Blurring average filter
    Mat output3;
    boxFilter(image,output3,image.type(),Size(dim,dim));

    //Median filter
    Mat output4;
    medianBlur(image,output4,dim);

    //Gaussian filter
    Mat output5;
    GaussianBlur(image,output5,Size(dim,dim),0,0);

    cv::imshow("Original image",image);
	cv::imshow("Filter2D image",output1);
	cv::imshow("Blur image",output2);
	cv::imshow("BoxFilter image",output3);
	cv::imshow("MedianBlur image",output4);
	cv::imshow("GaussianBlur image",output5);
	cv::waitKey(0);

	cout<<"Inserire threshold: ";
    cin>>th;

    //Thresholding 
	Mat mask;
	threshold(output5,mask,th,1,THRESH_BINARY_INV);

    Mat mask255;
    threshold(output5,mask255,th,255,THRESH_BINARY_INV);
    cv::imshow("Mask",mask255);

	//Immagine * maschera per mostrare solo le regioni di interesse
	cv::imshow("Image*Mask",image.mul(mask));

	cv::waitKey(0);
	return 0;
}