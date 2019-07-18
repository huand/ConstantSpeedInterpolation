// Copyright 2019 hubert andre alstom

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/videoio/videoio.hpp"

cv::Mat ComputeInterpFrame(cv::Mat frame1, cv::Mat frame2, double t);
cv::Mat frame1, frame2;
int main(int argc, char const* argv[]) {
  std::fstream infile("trajectory/id.txt");
  if (!infile) {
    std::cerr << "Could not open the id file!\n";
  }
  std::vector<double> i_id;
  double idd;
  while (infile >> idd) {
    i_id.push_back(idd);
  }

  cv::VideoCapture cap("/home/adlink/Videos/OneLap.mp4");

  cap >> frame2;
  // frame2.copyTo(frame1);
  frame1 = frame2.clone();
  cap >> frame2;
  int id = 1;
  int k = 0;
  cv::namedWindow("frame1");
  cv::namedWindow("frame2");
  cv::namedWindow("interp frame");
  cv::Mat iframe;
  for (auto&& i : i_id) {
    int id1 = floor(i) + 1;
    while (id != id1) {
      frame1 = frame2.clone();
      // frame2.copyTo(frame1);
      cap >> frame2;
      id++;
    }
    double t = i + 1 - id;

    iframe = ComputeInterpFrame(frame1, frame2, t);
    // ComputeInterpFrame(frame1, frame2, t);
    cv::imwrite(
        "/home/adlink/Videos/CefConstantFrames/" + std::to_string(k) + ".png",
        iframe);
    k++;
    cv::imshow("frame1", frame1);
    cv::imshow("frame2", frame2);
    cv::imshow("interp frame", iframe);
    std::cout << static_cast<double>(k) / i_id.size() << " %\n";
    std::cout << "t: " << t << std::endl;
    cv::waitKey(0);
  }
  return 0;
}
