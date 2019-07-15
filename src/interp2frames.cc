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

int main(int argc, char** argv) {
  cv::Mat flow, cflow, frame, iframe;
  cv::UMat gray, prevgray, uflow;
  cv::namedWindow("flow", 1);

  frame = cv::imread("/home/adlink/Pictures/1a.png");
  cvtColor(frame, prevgray, cv::COLOR_BGR2GRAY);
  frame = cv::imread("/home/adlink/Pictures/1b.png");
  cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
  calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
  cvtColor(prevgray, cflow, cv::COLOR_GRAY2BGR);
  uflow.copyTo(flow);
  drawOptFlowMap(flow, cflow, 16, 1.5, cv::Scalar(0, 255, 0));
  imshow("flow", cflow);

  cv::waitKey(0);
  return 0;
}
