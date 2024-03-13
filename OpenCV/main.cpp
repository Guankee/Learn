#include<iostream>
#include<opencv2/opencv.hpp>
int main(){
  cv::Mat src=cv::imread("E:\\Image\\biaoding1.jpg");
  cv::imshow("ff",src);
  cv::waitKey();

  return 0;
}