// Copyright 2019 hubert andre alstom

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
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
  frame1 = frame2.clone();
  cap >> frame2;
  int id = 0;
  int k = 0;
  cv::namedWindow("frame1");
  cv::namedWindow("frame2");
  cv::namedWindow("interp frame");
  int nite = 0;
  for (auto&& i : i_id) {
    nite++;
    int id1 = floor(i);
    while (id != id1) {
      std::swap(frame1, frame2);
      cap >> frame2;
      id++;
    }
    double t = i - id;

    cv::UMat uflow;
    cv::Mat gray1, gray2, flow;
    cvtColor(frame1, gray1, cv::COLOR_BGR2GRAY);
    cvtColor(frame2, gray2, cv::COLOR_BGR2GRAY);
    calcOpticalFlowFarneback(gray1, gray2, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
    uflow.copyTo(flow);
    cv::Mat iframe1;
    frame1.copyTo(iframe1);
    iframe1 = 0;
    // int count[frame1.rows][frame1.cols];
    std::vector<std::vector<int>> count1(frame1.rows,
                                         std::vector<int>(frame1.cols, 0));
    for (size_t y = 0; y < frame1.rows; y++) {
      for (size_t x = 0; x < frame1.cols; x++) {
        const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
        int tx = cvRound(x + fxy.x * t);
        int ty = cvRound(y + fxy.y * t);
        if (y + fxy.y < frame1.rows - 1 && x + fxy.x < frame1.cols - 1 &&
            y + fxy.y >= 0 && x + fxy.x >= 0) {
          cv::Vec3b color1 = frame1.at<cv::Vec3b>(y, x);
          cv::Vec3b color2 = frame2.at<cv::Vec3b>(y + fxy.y, x + fxy.x);
          cv::Vec3i newcolor = color1 * (1 - t) + color2 * t;
          cv::Vec3i mergecolor =
              cv::Vec3i(iframe1.at<cv::Vec3b>(ty, tx)) * count1[ty][tx] +
              newcolor;
          ++count1[ty][tx];
          iframe1.at<cv::Vec3b>(ty, tx) = mergecolor / count1[ty][tx];
          // iframe.at<cv::Vec3b>(ty, tx) = newcolor;
        }
      }
    }
    std::swap(gray1, gray2);
    std::swap(frame1, frame2);
    t = 1 - t;
    calcOpticalFlowFarneback(gray1, gray2, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
    uflow.copyTo(flow);
    cv::Mat iframe2;
    frame1.copyTo(iframe2);
    iframe2 = 0;
    // int count[frame1.rows][frame1.cols];
    std::vector<std::vector<int>> count2(frame1.rows,
                                         std::vector<int>(frame1.cols, 0));
    for (size_t y = 0; y < frame1.rows; y++) {
      for (size_t x = 0; x < frame1.cols; x++) {
        const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
        int tx = cvRound(x + fxy.x * t);
        int ty = cvRound(y + fxy.y * t);
        if (y + fxy.y < frame1.rows - 1 && x + fxy.x < frame1.cols - 1 &&
            y + fxy.y >= 0 && x + fxy.x >= 0) {
          cv::Vec3b color1 = frame1.at<cv::Vec3b>(y, x);
          cv::Vec3b color2 = frame2.at<cv::Vec3b>(y + fxy.y, x + fxy.x);
          cv::Vec3i newcolor = color1 * (1 - t) + color2 * t;
          cv::Vec3i mergecolor =
              cv::Vec3i(iframe2.at<cv::Vec3b>(ty, tx)) * count2[ty][tx] +
              newcolor;
          ++count2[ty][tx];
          iframe2.at<cv::Vec3b>(ty, tx) = mergecolor / count2[ty][tx];
          // iframe.at<cv::Vec3b>(ty, tx) = newcolor;
        }
      }
    }
    cv::Mat iframe;
    frame1.copyTo(iframe);  // TODO(hubert) optimize
    iframe = 0;
    for (int y = 0; y < frame1.rows; y++) {
      for (int x = 0; x < frame1.cols; x++) {
        int nc = static_cast<int>(count1[y][x] > 0) +
                 static_cast<int>(count2[y][x] > 0);
        if (nc > 0) {
          iframe.at<cv::Vec3b>(y, x) =
              iframe1.at<cv::Vec3b>(y, x) / nc +
              iframe2.at<cv::Vec3b>(y, x) / nc;  // TODO(hubert): lossless
        } else {
          iframe.at<cv::Vec3b>(y, x) = frame1.at<cv::Vec3b>(y, x) * (1 - t) +
                                       frame2.at<cv::Vec3b>(y, x) * t;
        }
      }
    }
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    // iframe = ComputeInterpFrame(frame1.clone(), frame2.clone(), t);
    // ComputeInterpFrame(frame1, frame2, t);
    cv::imwrite("/home/adlink/Videos/CefConstantFrames/" +
                    std::to_string(nite) + ".png",
                iframe);
    k++;
    // cv::imshow("frame1", frame1);
    // cv::imshow("frame2", frame2);
    // cv::imshow("interp frame", iframe);
    std::cout << static_cast<double>(k) / i_id.size() << " %\n";
    std::cout << "t: " << t << std::endl;
    cv::waitKey(1);
  }
  return 0;
}
