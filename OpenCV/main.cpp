#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/aruco.hpp>
//#include<opencv2/a>
#include<memory>
using namespace cv;
int main() {
	cv::Mat src = cv::imread("E:\\Image\\biaoding1.jpg");
	// aruco::
	cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
	cv::Ptr<cv::aruco::Dictionary> dictionary = std::make_shared<cv::aruco::Dictionary>(dict);
	// dict.writeDictionary("E:\\Image\\dict.jpg",);
	  // cv::imshow("ff", src);
	  // cv::waitKey();
   Mat markerImage; 
   dict.generateImageMarker(5,800,markerImage);
  //  cv::aruco::drawCharucoDiamond(dict, 10, 1, 1, markerImage);
	// std::vector<int> marker_ids;
	// std::vector<std::vector<cv::Point2f>> marker_corners;
	// cv::aruco::DetectorParameters parameters;
	// parameters.refine_corners = true;
	// cv::aruco::detectMarkers(src, dictionary, marker_corners, marker_ids);
	 cv::imshow("ff", markerImage);
	 cv::waitKey();
	return 0;
}