#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

double smthreshold;
int tsize;
int media;

class TNode{
    private:
        Rect region;
        TNode *UL, *UR, *LL, *LR;
        vector<TNode*> merged;
        vector<bool> mergedB = vector<bool>(4, false);

        double stddev, mean;
    
    public:

        TNode(Rect R){
            region = R;
            UL = UR = LL = LR = nullptr;
        };

        Rect &getRegion(){return region;};

        TNode *getUL(){return UL;};
        TNode *getUR(){return UR;};
        TNode *getLL(){return LL;};
        TNode *getLR(){return LR;};

        void setUL(TNode *N){UL = N;};
        void setUR(TNode *N){UR = N;};
        void setLL(TNode *N){LL = N;};
        void setLR(TNode *N){LR = N;};

        vector<TNode *> &getMeregd(){return merged;};
        void addRegion(TNode* R){merged.push_back(R);};

        bool getMergedB(int i){return mergedB[i];};
        void setMergedB(int i){mergedB[i] = true;};

        double getStdDev(){return stddev;};
        void setStdDev(double stddev){this->stddev = stddev;};

        double getMean(){return mean;};
        void setMean(double mean){this->mean = mean;};
};

TNode *split(Mat& src, Rect R){

    TNode *root = new TNode(R);
    Scalar stddev, mean;
    meanStdDev(src(R), mean, stddev);

    root->setStdDev(stddev[0]);
    root->setMean(mean[0]);

    if(R.width > tsize && root->getStdDev() > smthreshold){

        Rect ul(R.x, R.y, R.height/2, R.width/2);
        root->setUL(split(src, ul));


        Rect ur(R.x, R.y+R.width/2, R.height/2, R.width/2);
        root->setUR(split(src, ur));


        Rect ll(R.x+R.height/2, R.y, R.height/2, R.width/2);
        root->setLL(split(src, ll));


        Rect lr(R.x+R.height/2, R.y+R.width/2, R.height/2, R.width/2);
        root->setLR(split(src, lr));
    }

    rectangle(src, R, Scalar(0));
    return root;
}

void merge(TNode *root){

    if(root->getRegion().width > tsize && root->getStdDev() > smthreshold){
        //UL - UR
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
        //UR - LR
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
        //LL - LR
        else if(root->getLL()->getStdDev() <= smthreshold && root->getLR()->getStdDev() <= smthreshold && abs(root->getLL()->getMean() - root->getLR()->getMean()) < media){
            root->addRegion(root->getLL());
            root->addRegion(root->getLR());
            root->setMergedB(2);
            root->setMergedB(3);

            if(root->getUL()->getStdDev() <= smthreshold && root->getUR()->getStdDev() <= smthreshold && abs(root->getUL()->getMean() - root->getUR()->getMean()) < media){
                root->addRegion(root->getUL());
                root->addRegion(root->getUR());
                root->setMergedB(0);
                root->setMergedB(1);    
            
            }
            else{
                merge(root->getUL());
                merge(root->getUR());
            }
        }
        //UL - LL
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
        else{
            merge(root->getUL());
            merge(root->getUR());
            merge(root->getLL());
            merge(root->getLR());
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

void segment(Mat& src, TNode* root){

    vector<TNode* > tmp = root->getMeregd();

    if(!tmp.size()){
        segment(src, root->getUL());
        segment(src, root->getUR());
        segment(src, root->getLL());
        segment(src, root->getLR());
    }
    else{

        double val = 0;
        for(auto x:tmp){
            val += x->getMean();
        }
        val /= tmp.size();

        for(auto x:tmp){
            src(root->getRegion()) = val;
        }

        if(tmp.size() > 1){

            if( !root->getMergedB(0) )
                segment(src, root->getUL());
            if( !root->getMergedB(1) )
                segment(src, root->getUR());
            if( !root->getMergedB(2) )
                segment(src, root->getLR());
            if( !root->getMergedB(3) )
                segment(src, root->getLL());
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

    int exponent = log(min(src.cols, src.rows)) / log(2);
    int s = pow(2.0, exponent);

    Rect square = Rect(0,0,s,s);
    src = src(square).clone();

    Mat srcSeg=src.clone();

    TNode *root = split(src, Rect(0,0, src.rows, src.cols));
    merge(root);
    segment(srcSeg, root);

    imshow("Quad Tree", src);
    imshow("Segmented", srcSeg);
    waitKey(0);
    


    return 0;
}
