// -*- C++ -*-
#ifndef _REAL_SENSE_H_
#define _REAL_SENSE_H_

#include <librealsense2/rs.hpp>
#include <librealsense2/rs_advanced_mode.hpp>
#include <iostream>
class CRealSense {
public:
  //! Default constructor
  CRealSense();

  //! Destructor
  virtual ~CRealSense();

  int init(const int width, const int height);
  int stop();
  int prepare(int times = 30);
  int getRGBFrame(rs2::frame &rgbFrame);
  int getRGBIntrinsic(double *intrinsics);

  rs2::frameset frames; // 相机预热-删除几个第一帧让自动曝光稳定。

  int runState;
  
  double depth_scale;
  double depth_factor;
  int imageWidth, imageHeight;
private:
  rs2::pipeline pipe;
  rs2::config cfg; //创建一个以非默认配置的配置用来配置管道
  rs2::pipeline_profile profile;
};

#endif // _REAL_SENSE_H_