// Copyright 2019 Hubert Andre - Alstom

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/videoio.hpp"

#include <iostream>

static void drawOptFlowMap(const cv::Mat& flow, cv::Mat& cflowmap, int step,
                           double, const cv::Scalar& color) {
  for (int y = 0; y < cflowmap.rows; y += step)
    for (int x = 0; x < cflowmap.cols; x += step) {
      const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
      line(cflowmap, cv::Point(x, y),
           cv::Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), color);
      circle(cflowmap, cv::Point(x, y), 2, color, -1);
    }
}

static cv::Mat ComputeInterpFrame(const cv::Mat& frame1, const cv::Mat& frame2,
                                  const cv::Mat& flow, const double& t) {
  cv::Mat iframe_12 = frame1 / 2 + frame2 / 2;
  cv::Mat iframe(frame1.rows, frame1.cols, CV_8UC3);
  cv::Mat count(frame1.rows, frame1.cols, CV_8U);
  for (size_t y = 0; y < frame1.rows; y++) {
    for (size_t x = 0; x < frame1.cols; x++) {
      const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
      cv::Vec3b color1 = frame1.at<cv::Vec3b>(y, x);
      cv::Vec3b color2 = frame2.at<cv::Vec3b>(int(y + fxy.y), int(x + fxy.x));
      if (int(y + fxy.y * t) < frame1.rows &&
          int(x + fxy.x * t) < frame1.cols && int(y + fxy.y * t) >= 0 &&
          int(x + fxy.x * t) >= 0) {
        count.at<char>(int(y + fxy.y * t), int(x + fxy.x * t)) =
            1;  // TODO: check in paper if its normal that no double source

        iframe.at<cv::Vec3b>(int(y + fxy.y * t), int(x + fxy.x * t)) =
            color1 * (1. - t) + color2 * t;
      }
    }
  }
  for (size_t y = 0; y < frame1.rows; y++) {
    for (size_t x = 0; x < frame1.cols; x++) {
      if (count.at<char>(y, x) == 0) {
        iframe.at<cv::Vec3b>(y, x) = frame1.at<cv::Vec3b>(y, x) * (1 - t) +
                                     frame2.at<cv::Vec3b>(y, x) * t;
      }
    }
  }
  return iframe;
}
int main(int argc, char** argv) {
  cv::Mat flow, cflow, frame1, frame2;
  cv::UMat gray, prevgray, uflow;
  cv::namedWindow("flow", 1);
  frame1 = cv::imread("/home/home/Pictures/1a.png");
  cvtColor(frame1, prevgray, cv::COLOR_BGR2GRAY);
  frame2 = cv::imread("/home/home/Pictures/1b.png");
  cvtColor(frame2, gray, cv::COLOR_BGR2GRAY);
  calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
  cvtColor(prevgray, cflow, cv::COLOR_GRAY2BGR);
  uflow.copyTo(flow);  // TODO: straight to flow
  drawOptFlowMap(flow, cflow, 16, 1.5, cv::Scalar(0, 255, 0));
  imshow("flow", cflow);
  cv::Mat iframe = ComputeInterpFrame(frame1, frame2, flow, 0.5);
  imshow("interpolated frame", iframe);
  cv::waitKey(0);
  return 0;
}
