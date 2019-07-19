// Copyright 2019 Hubert andre alstom

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"

int main(int argc, char const *argv[]) {
  const int fps = 30;
  const double dt = 1. / fps;
  double curr_position = 0;
  double speed = 550;
  const double step = 0.04;
  std::string folder_path = "/home/adlink/Videos/CefConstantFrames/";
  std::string window_name = "Controlled Video";
  cv::namedWindow(window_name);
  auto time_0 = std::chrono::system_clock::now();
  int count = 0;
  while (1) {
    count++;
    curr_position += speed * dt;
    int frame_id = curr_position / step + 1;
    std::string frame_name = std::to_string(frame_id) + ".png";
    std::string frame_path = folder_path + frame_name;
    cv::Mat frame = cv::imread(frame_path);
    cv::imshow(window_name, frame);
    auto time_now = std::chrono::system_clock::now();
    auto time_elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(time_now -
                                                              time_0);
    int wait_ms = dt * 1000 * count - time_elapsed_ms.count();
    if (wait_ms < 1) wait_ms = 1;
    cv::waitKey(wait_ms);
  }

  return 0;
}
