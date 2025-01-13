#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
std::vector<cv::Point3d> getBoardCoordinate(
    const float arucoSize, const float gapSize, const int rows,
    const int columns, const std::vector<int>& markerList) {
  static std::vector<cv::Point2f> boardZeroList;
  boardZeroList.clear();

  cv::Point2f tempXY;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      tempXY.x = i * (arucoSize + gapSize);
      tempXY.y = j * (arucoSize + gapSize);
      boardZeroList.push_back(tempXY);
    }
  }

  std::vector<cv::Point3d> boardCoordinateList;
  cv::Point3f tempXYZ;
  static int indexList[8] = {0, 0, 0, 1, 1, 1, 1, 0};
  for (int k = 0; k < markerList.size(); ++k) {
    for (int k1 = 0; k1 < 4; ++k1) {
      tempXYZ.x =
          boardZeroList[markerList[k]].x + indexList[k1 * 2] * arucoSize;
      tempXYZ.y =
          boardZeroList[markerList[k]].y + indexList[k1 * 2 + 1] * arucoSize;
      tempXYZ.z = 0.0;
      boardCoordinateList.push_back(tempXYZ);
    }
  }

  return boardCoordinateList;
}
int main() {
  // cv::aruco::Dictionary dictionary =
  // cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

  // cv::aruco::CharucoBoard board(cv::Size(3, 4), 0.08, 0.07, dictionary);
  // cv::Size size(600, 800);
  // cv::Mat image;
  // board.generateImage(size, image);
  // std::vector<cv::Point3f> corner = board.getChessboardCorners();
  // cv::Ptr<cv::aruco::Dictionary> dic =
  // std::make_shared<cv::aruco::Dictionary>(board); std::vector<int>
  // marker_ids; std::vector<std::vector<cv::Point2f>> marker_corners;
  // cv::aruco::detectMarkers(image, dic, marker_corners, marker_ids);
  // cv::imwrite("E:\\Image\\board.png", image);

  std::vector<int> marker_train_list = {1,  2,  3,  4,  5,  6,  7,  8,  9,  11,
                                        13, 15, 16, 17, 18, 19, 20, 21, 22, 23};

  std::vector<int> marker_test_list = {0, 10, 12, 14, 24};
  std::vector<cv::Point3d> bc_train_list =
      getBoardCoordinate(20.0, 5.0, 5, 5, marker_train_list);
  std::vector<cv::Point3d> bc_test_list =
      getBoardCoordinate(20.0, 5.0, 5, 5, marker_test_list);

  cv::Mat src = cv::imread("E:\\Image\\biaoding55.jpg");
  cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
  cv::Ptr<cv::aruco::Dictionary> dictPtr = cv::makePtr<cv::aruco::Dictionary>(dictionary);
  //cv::aruco::detectMarkers();
  std::vector<int> marker_ids;
  std::vector<std::vector<cv::Point2f>> marker_corners;
  cv::aruco::detectMarkers(src, dictPtr, marker_corners, marker_ids);
  cv::Mat outputImage = src.clone();
  cv::aruco::drawDetectedMarkers(outputImage, marker_corners, marker_ids);

  cv::Mat pc_list_all = cv::Mat::zeros(25 * 4, 3, CV_64FC1);
  cv::Mat idAssertList = cv::Mat::ones(25, 1, CV_8UC1);
  return 0;
}