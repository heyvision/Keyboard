#ifndef _IMAGE_PROC_H_
#define _IMAGE_PROC_H_

#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <X11/XKBlib.h>
#include <vector>
using namespace std;
using namespace cv;
class CImageProc {
public:
  CImageProc();
  virtual ~CImageProc();

  int GetButton(Mat& img, string str, vector<Point2f>& points);
  int SaveImage(Mat& img);
  const vector<vector<float>> GetRelativePos() const;
  int CalcuHomography(Mat& img, Mat& tempImg);
private:
  Mat m_homography;
};

#endif // _IMAGE_PROC_H_
