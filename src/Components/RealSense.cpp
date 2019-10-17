#include "RealSense.h"

CRealSense::CRealSense() {
  runState = 0;
  depth_scale = 1000;
  imageWidth = 1280;
  imageHeight = 720;
}
CRealSense::~CRealSense() {}

int CRealSense::prepare(int times) {
  // if (runState == 0) return 0;
  if (times <= 0) times = 30;
  if (times >= 1000) times = 1000;
  for (int i = 0; i < times; i++) {
    pipe.wait_for_frames();
  }
  return 1;
}

int CRealSense::init(const int width, const int height) {
  if (runState != 0) return 0;
  imageWidth = width;
  imageHeight = height;
  // rs2::pipeline pipe;
  // rs2::config cfg;
  cfg.enable_stream(RS2_STREAM_COLOR, width, height, RS2_FORMAT_BGR8, 30); //向配置添加所需的流
  // cfg.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);//向配置添加所需的流
  profile = pipe.start(cfg);
  runState = 0;
  prepare();

  return 1;
}


int CRealSense::getRGBFrame(rs2::frame &rgbFrame) {
  rs2::frameset frames;
  //if (runState == 0) return 0;
  frames = pipe.wait_for_frames();
  rgbFrame = frames.get_color_frame();
  return 1;
}

int CRealSense::getRGBIntrinsic(double *intrinsics)
{
  //rs2::pipeline_profile profile;
  //if (runState == 0) return 0;
  auto color_stream = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
  auto intrin = color_stream.get_intrinsics();
  double m_intrinsics[4];
  m_intrinsics[0] = intrin.fx;
  m_intrinsics[1] = intrin.fy;
  m_intrinsics[2] = intrin.ppx;
  m_intrinsics[3] = intrin.ppy;
  intrinsics = m_intrinsics;
  return 1;
}

int CRealSense::stop() {
  //  cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30); //向配置添加所需的流
  //  pipe.start(cfg); // 指示管道使用所请求的配置启动流
  //if (runState == 0) return 0;
  rs2::pipeline pipe;
  pipe.stop();
  //cfg.disable_all_streams();
  runState = 0;
  return 1;
}

