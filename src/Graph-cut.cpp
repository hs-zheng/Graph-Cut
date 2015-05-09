/*
 作者 郑鸿昇
 创建日期 2015/5/9 21:25
 备注 使用grabcut进行
 */
#include "Graph-cut.h"

// 重置变量
void GraphCut::reset() {
    if (!_mask.empty()) {
        _mask.setTo(Scalar::all(GC_BGD));
    }
    
    _bgdPxls.clear();
    _fgdPxls.clear();
    _prBgdPxls.clear();
    _prFgdPxls.clear();
    
    _isInitialized = false;
    _rectState = 0;
    _lblsState = 0;
    _prLblsState = 0;
    _iterCount = 0;
}

// 设置图片和窗口名
void GraphCut::setImageAndWinName(const Mat &image, const string & winName) {
    if (image.empty() || winName.empty()) {
        cout<< "设置图片和窗口名失败， 请检查是否传入空值！！！ ";
        return ;
    }
    _image = &image;
    _winName = &winName;
    _mask.create(_image->size(), CV_8UC1);
    reset(); 
}

void GraphCut::showImage() const {
    if (_image->empty() || _winName == NULL) {
        cout<< "图片或窗口名为空， 请检查！！！ ";
        return;
    }
    Mat res;
    Mat binMask;
    if (!_isInitialized) {
        _image->copyTo(res);
    } else {
      getBinMask(_mask, binMask);
      _image->copyTo(res, binMask);  
    }
    vector<Point>::const_iterator it;
    for (it = _bgdPxls.begin(); it != _bgdPxls.end(); ++it) {
        circle(res, *it, _radius, BLUE, _thickness);
    }
    for (it = _fgdPxls.begin(); it != _fgdPxls.end(); ++it) {
        circle(res, *it, _radius, RED, _thickness);
    }
	for (it = _prBgdPxls.begin(); it != _prBgdPxls.end(); ++it) {
        circle(res, *it, _radius, LIGHTBLUE, _thickness);
    }		
	for (it = _prFgdPxls.begin(); it != _prFgdPxls.end(); ++it) {
        circle(res, *it, _radius, PINK, _thickness);
    }
	if (_rectState == 1 || _rectState == 2) {
        rectangle(res, Point(_rect.x, _rect.y), Point(_rect.x + _rect.width, _rect.y + _rect.height), GREEN, 2);
    }	
    imshow(*_winName, res);
}

void GraphCut::setRectInMask() {
    assert(!_mask.empty());
    _mask.setTo(GC_BGD);
    _rect.x = max(0, _rect.x);
    _rect.y = max(0, _rect.y);
    _rect.width = min(_rect.width, _image->cols - _rect.x);
    _rect.height = min(_rect.height, _image->rows - _rect.y);
    (_mask(_rect)).setTo(Scalar(GC_PR_FGD));
}

void GraphCut::setLblsInMask(int flags, Point p, bool isPr) {
    vector<Point> *bpxls, *fpxls;
    uchar bvalue, fvalue;
    if (!isPr) {
        bpxls = &_bgdPxls;
        fpxls = &_fgdPxls;
        bvalue = GC_BGD;
        fvalue = GC_FGD;
    } else {
        bpxls = &_prBgdPxls;
        fpxls = &_prFgdPxls;
        bvalue = GC_PR_BGD;
        fvalue = GC_PR_FGD;
    }
    if (flags & BGD_KEY) {
        bpxls->push_back(p);
        circle(_mask, p, _radius, fvalue, _thickness);
    }
    if( flags & FGD_KEY )
	{
		fpxls->push_back(p);
		circle(_mask, p, _radius, fvalue, _thickness );   //该点处为3
	}
}

void GraphCut::mouseClick(int event, int x, int y, int flags, void * param) {
    switch(event) {
        case CV_EVENT_LBUTTONDOWN: {
            bool isb = (flags & BGD_KEY) != 0,
                isf = (flags & FGD_KEY) != 0;
            if (_rectState == 0 && !isb && !isf) {
                _rectState = 1;
                _rect = Rect(x, y, 1, 1);
            }
            if ((isb || isf) && _rectState == 2) {
                _lblsState = 1;
            }
            break;
        }
        case CV_EVENT_RBUTTONDOWN: {
            bool isb = (flags & BGD_KEY) != 0,
				isf = (flags & FGD_KEY) != 0;
			if ((isb || isf) && _rectState == 2) {
                _prLblsState = 1;   
            }
            break;
        }
        case CV_EVENT_LBUTTONUP: {
            if (_rectState == 1 ) {
    			_rect = Rect(Point(_rect.x, _rect.y), Point(x,y));   //矩形结束
    			_rectState = 2;
    			setRectInMask();
    			assert( _bgdPxls.empty() && _fgdPxls.empty() && _prBgdPxls.empty() && _prFgdPxls.empty() );
    			showImage();
    		}
    		if (_lblsState == 1) {
    			setLblsInMask(flags, Point(x,y), false);    //画出前景点
    			_lblsState = 2;
    			showImage();
    		}
    		break;
        }
        case CV_EVENT_MOUSEMOVE: {
            if( _rectState == 1 ) {
    			_rect = Rect(Point(_rect.x, _rect.y), Point(x,y));
    			assert(_bgdPxls.empty() && _fgdPxls.empty() && _prBgdPxls.empty() && _prFgdPxls.empty() );
    			showImage();    //不断的显示图片
    		}
    		else if (_lblsState == 1) {
    			setLblsInMask(flags, Point(x,y), false);
    			showImage();
    		}
    		else if (_prLblsState == 1) {
    			setLblsInMask(flags, Point(x,y), true);
    			showImage();
    		}
    		break;
        }
    }
}

int GraphCut::nextIter() {
    if (_isInitialized) {
        grabCut(*_image, _mask, _rect, _bgdModel, _fgdModel, 1);        
    } else {
        if (_rectState != 2) {
            return _iterCount;
        }
        if (_lblsState == 2 || _prLblsState == 2) {
            grabCut(*_image, _mask, _rect, _bgdModel, _fgdModel, 1, GC_INIT_WITH_MASK);
        } else {
            grabCut( *_image, _mask, _rect, _bgdModel, _fgdModel, 1, GC_INIT_WITH_RECT);
        }
        _isInitialized = true;
    }
    _iterCount ++;
    _bgdPxls.clear();
    _fgdPxls.clear();
    _prBgdPxls.clear();
    _prFgdPxls.clear();
    
    return _iterCount;
}