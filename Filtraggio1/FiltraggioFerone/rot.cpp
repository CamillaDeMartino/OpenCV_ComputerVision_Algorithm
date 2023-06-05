#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace cv;

void printFilter(Mat filter){
    for(int i=0;i<filter.rows;i++){
        for(int j=0;j<filter.cols;j++)
            cout<<setw(2)<<filter.at<int>(i,j)<<" ";
        cout<<endl;
    }
    cout<<endl;
}

int main( int argc, char** argv ){

	//int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
    Mat filter = Mat(5,5,CV_32S,data);
    Mat r_filter;

    cout << "Matrice originale"<<endl;
    printFilter(filter);

    cout << "Matrice ruotata di 90° in senso orario"<<endl;
    rotate(filter,r_filter,ROTATE_90_CLOCKWISE);
    printFilter(r_filter);

    cout << "Matrice ruotata di 180°"<<endl;
    rotate(filter,r_filter,ROTATE_180);
    printFilter(r_filter);

    cout << "Matrice ruotata di 90° in senso antiorario"<<endl;
    rotate(filter,r_filter,ROTATE_90_COUNTERCLOCKWISE);
    printFilter(r_filter);

	return 0;
}
