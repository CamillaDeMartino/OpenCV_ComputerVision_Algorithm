#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<float> normHist(const Mat& src){

    vector<float> hist(256, 0.0f);

    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){
            hist.at(src.at<uchar>(x,y))++;
        }
    }

    for(int i = 0; i < 256; i++)
        hist.at(i) /= src.rows * src.cols;

    return hist;
}

float globalAvg(vector<float> hist){

    float globAvg = hist.at(0);

    for(int i = 1; i < 256; i++)
        globAvg += i * hist.at(i);
    
    return globAvg;
}

vector<int> kStar(vector<float> hist, float globAvg){

    vector<float> prob(3, 0.0f);
    vector<float> cumAvg(3, 0.0f);
    vector<int> kstar(2, 0);
    float maxVariance = 0.0f;

    for(int i = 0; i < 254; i++){

        prob.at(0) += hist.at(i);
        cumAvg.at(0) += i * hist.at(i);

        for(int j = i+1; j < 255; j++){

            prob.at(1) += hist.at(j);
            cumAvg.at(1) += j*hist.at(j);

            for(int k = j+1; k < 256; k++){

                prob.at(2) += hist.at(k);
                cumAvg.at(2) += k * hist.at(k);

                float sigma = 0.0f;
                for(int z = 0; z < 3; z++)
                    sigma += prob.at(z) * pow((cumAvg.at(z)/prob.at(z) - globAvg), 2);

                if(sigma > maxVariance){
                    maxVariance = sigma;
                    kstar.at(0) = i;
                    kstar.at(1) = j;
                }
            }
            
            prob.at(2) = cumAvg.at(2) = 0.0f;
        }
        prob.at(1) = cumAvg.at(1) = 0.0f;
    }

    return kstar;
}

void myThreshold(Mat& src, Mat& dst, vector<int> k){
    
    dst = Mat::zeros(src.size(), src.type());

    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){
            if(src.at<uchar>(x,y) >= k.at(1))
                dst.at<uchar>(x,y) = 255;
            else if(src.at<uchar>(x,y) >= k.at(0))
                dst.at<uchar>(x,y) = 127;
        }
    }


}

void otsu(const Mat& src, Mat& dst){
    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    vector<float> hist = normHist(blur);
    float globAvg = globalAvg(hist);
    vector<int> k = kStar(hist, globAvg);
    myThreshold(blur, dst, k);

}


int main(int argc, char** argv){

    Mat src, dst;
    src = imread(argv[1], IMREAD_GRAYSCALE);

    if( src.empty() ){
        return -1;
    }

    imshow("Original", src);

    otsu(src, dst);
    imshow("Otsu", dst);
    waitKey(0);


    return 0;

}