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

cv::Mat ComputeInterpFrame(cv::Mat frame1, cv::Mat frame2, double t) {
  //////
  cv::Mat flow;
  cv::UMat gray, prevgray;
  cvtColor(frame1, prevgray, cv::COLOR_BGR2GRAY);
  cvtColor(frame2, gray, cv::COLOR_BGR2GRAY);
  cv::Mat count(frame1.rows, frame1.cols, CV_8U);
  count = 0;
  cv::Mat iframe(frame1.rows, frame1.cols, CV_8UC3);
  iframe = 0;
  ////////////////////////////////////////////////////////////////////////
  calcOpticalFlowFarneback(prevgray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
  for (size_t y = 0; y < frame1.rows; y++) {
    for (size_t x = 0; x < frame1.cols; x++) {
      const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
      int tx = cvRound(x + fxy.x * t);
      int ty = cvRound(y + fxy.y * t);
      if (y + fxy.y < frame1.rows && x + fxy.x < frame1.cols &&
          y + fxy.y >= 0 && x + fxy.x >= 0) {
        cv::Vec3b color1 = frame1.at<cv::Vec3b>(y, x);
        cv::Vec3b color2 = frame2.at<cv::Vec3b>(y + fxy.y, x + fxy.x);
        cv::Vec3i newcolor = color1 * (1 - t) + color2 * t;
        cv::Vec3i mergecolor =
            cv::Vec3i(iframe.at<cv::Vec3b>(ty, tx)) * count.at<char>(ty, tx) +
            newcolor;
        ++count.at<char>(ty, tx);
        iframe.at<cv::Vec3b>(ty, tx) = mergecolor / count.at<char>(ty, tx);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////

  cv::swap(gray, prevgray);
  calcOpticalFlowFarneback(gray, prevgray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
  // t = 1 - t;
  for (size_t y = 0; y < frame1.rows; y++) {
    for (size_t x = 0; x < frame1.cols; x++) {
      const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
      int tx = cvRound(x + fxy.x * t);
      int ty = cvRound(y + fxy.y * t);
      if (y + fxy.y < frame1.rows && x + fxy.x < frame1.cols &&
          y + fxy.y >= 0 && x + fxy.x >= 0) {
        cv::Vec3b color1 = frame1.at<cv::Vec3b>(y, x);
        cv::Vec3b color2 = frame2.at<cv::Vec3b>(y + fxy.y, x + fxy.x);
        cv::Vec3i newcolor = color1 * (1 - t) + color2 * t;
        cv::Vec3i mergecolor =
            cv::Vec3i(iframe.at<cv::Vec3b>(ty, tx)) * count.at<char>(ty, tx) +
            newcolor;
        ++count.at<char>(ty, tx);
        iframe.at<cv::Vec3b>(ty, tx) = mergecolor / count.at<char>(ty, tx);
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
  return iframe.clone();
}
// int main(int argc, char** argv) {
//  cv::Mat frame1, frame2;
//  frame1 = cv::imread("/home/home/Pictures/1a.png");
//  frame2 = cv::imread("/home/home/Pictures/1b.png");
//  cv::Mat iframe = ComputeInterpFrame(frame1, frame2, 0.35);
//  imshow("interpolated frame", iframe);
//  cv::waitKey(0);
//  return 0;
//}
