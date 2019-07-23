// Copyright 2019 Hubert andre alstom

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include "opencv2/highgui/highgui.hpp"

#include "opencv2/videoio/videoio.hpp"
int accel = 50;
double acc_max = 0.2;
double acc_min = -1;
static void on_trackbar(int, void *) {
  // std::cout << "new accel: " << accel << "\tkm/h" << std::endl;
}

int main(int argc, char const *argv[]) {
  const int fps = 30;
  const double dt = 1. / fps;
  double curr_position = 0;
  double speed = 0;
  const double step = 0.04;
  std::string folder_path = "/home/adlink/Videos/CefConstantFrames/";
  std::string window_name = "Controlled Video";
  cv::namedWindow(window_name);
  cv::createTrackbar("accel [km/h]", window_name, &accel, 100, on_trackbar);
  auto time_0 = std::chrono::system_clock::now();
  int count = 0;
  while (1) {
    count++;
    double accel_curr = accel > 50 ? (accel - 50.) / 50 * acc_max
                                   : -(accel - 50.) / 50. * acc_min;
    std::cout << accel_curr << std::endl;

    // double accel_curr =
    //   (static_cast<double>(accel) / 100. * (2 * acc_max) - acc_max);
    speed += accel_curr * dt;
    if (speed < 0) speed = 0;
    curr_position += speed * dt;
    if (curr_position > 1828) curr_position -= 1828;
    if (curr_position < 0) curr_position += 1828;

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
