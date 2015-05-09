/*
 作者 郑鸿昇
 创建日期 2015/5/9 21:25
 备注 Graph-Cut 算法头部信息
 */

#ifndef GRAPH_CUT_H
#define GRAPH_CUT_H
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define RED Scalar(0, 0, 255)
#define PINK Scalar(230, 130, 255)
#define BLUE Scalar(255, 0, 0)
#define LIGHTBLUE Scalar(255, 255, 160)
#define GREEN Scalar(0, 255, 0)


class GraphCut {
public:
    void reset();
    void setImageAndWinName(const Mat & image, const string & winName);
    void showImage() const;
    void mouseClick(int event, int x, int y, int flags, void * param);
    int nextIter();
    int getIterCount() const { return _iterCount; }
    
private:
    void setRectInMask();
    void setLblsInMask(int flags, Point p, bool isPr);
    
    static void getBinMask( const Mat& comMask, Mat& binMask )
    {
    	if( comMask.empty() || comMask.type()!=CV_8UC1 )
    		CV_Error( CV_StsBadArg, "comMask is empty or has incorrect type (not CV_8UC1)" );
    	if( binMask.empty() || binMask.rows!=comMask.rows || binMask.cols!=comMask.cols )
    		binMask.create( comMask.size(), CV_8UC1 );
    	binMask = comMask & 1;  //得到mask的最低位,实际上是只保留确定的或者有可能的前景点当做mask
    }
    
    const string * _winName;
    const Mat * _image;
    static const int BGD_KEY = CV_EVENT_FLAG_CTRLKEY;
    static const int FGD_KEY = CV_EVENT_FLAG_SHIFTKEY;
    
    static const int _radius = 2;
    static const int _thickness = -1;
    
    Mat _mask;
    Mat _bgdModel, _fgdModel;
    
    uchar _rectState, _lblsState, _prLblsState;
    bool _isInitialized;
    
    Rect _rect;
    vector<Point> _fgdPxls, _bgdPxls, _prFgdPxls, _prBgdPxls;
	int _iterCount; 
};

#endif