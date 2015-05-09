/*
 作者 郑鸿昇
 创建日期 2015/5/9 21:25
 备注 程序入口文件
 */
#include <opencv2/opencv.hpp>
#include "Graph-cut.h"

using namespace cv;

GraphCut graphCut;

static void onMouse(int event, int x, int y, int flags, void * param) {
    graphCut.mouseClick(event, x, y, flags, param);
}

int main(int argc, char ** argv) {
    string fileName = "images/test.jpg";
//    cout<< "请输入图片： ";
//    cin>>fileName;
    
    Mat image = imread(fileName, 1);
    if (image.empty()) {
        cout<<"无法找到图片"<<endl;
        return 1;
    }
    
    const string winName = "image";
    imshow( winName.c_str(), CV_WINDOW_AUTOSIZE );
    cvSetMouseCallback(winName.c_str(), onMouse, 0);
    graphCut.setImageAndWinName(image, winName);
    graphCut.showImage();
    
    
    for(;;)
	{
		int c = cvWaitKey(0);
		switch( (char) c )
		{
		case '\x1b':
			cout << "Exiting ..." << endl;
			goto exit_main;
		case 'r':
			cout << endl;
			graphCut.reset();
			graphCut.showImage();
			break;
		case 'n':
			int iterCount = graphCut.getIterCount();
			cout << "<" << iterCount << "... ";
			int newIterCount = graphCut.nextIter();
			if( newIterCount > iterCount )
			{
				graphCut.showImage();
				cout << iterCount << ">" << endl;
			}
			else
				cout << "rect must be determined>" << endl;
			break;
		}
	}

exit_main:
	return 0;
}