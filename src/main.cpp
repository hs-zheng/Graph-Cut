#include <opencv2/opencv.hpp>
#include "Graph-cut.h"

using namespace cv;

GraphCut graphCut;

static void onMouse(int event, int x, int y, int flags, void * param) {
    graphCut.mouseClick(event, x, y, flags, param);
}

int main(int argc, char ** argv) {
    string fileName;
    cout<< "请输入图片： ";
    cin>>fileName;
    
    Mat image = imread(fileName, 1);
    if (image.empty()) {
        cout<<"无法找到图片"<<endl;
        return 1;
    }
    
    imshow("iamge", CV_WINDOW_AUTOSIZE);
    cvSetMouseCallback("image", onMouse, 0);
    
    graphCut.setImageAndWinName(image, "image");
    
    cvWaitKey(0);
}