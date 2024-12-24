#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/aruco.hpp>
//#include<opencv2/a>
#include<memory>
//using namespace cv;

cv::Mat rotateImage(const cv::Mat& src_img, const int degree)
{
	if (degree == 90) {
		cv::Mat srcCopy = cv::Mat(src_img.rows, src_img.cols, src_img.depth());
		cv::transpose(src_img, srcCopy);
		cv::flip(srcCopy, srcCopy, 1);
		return srcCopy;
	}
	else if (degree == 180) {
		cv::Mat srcCopy = cv::Mat(src_img.rows, src_img.cols, src_img.depth());
		cv::flip(src_img, srcCopy, -1);
		return srcCopy;
	}
	else if (degree == 270) {
		cv::Mat srcCopy = cv::Mat(src_img.rows, src_img.cols, src_img.depth());
		cv::transpose(src_img, srcCopy);
		cv::flip(srcCopy, srcCopy, 0);
		return srcCopy;
	}
	else {
		return src_img;
	}
}
std::vector<cv::Point> find255Coordinates(const cv::Mat& mat)
{
	std::vector<cv::Point> coordinates;

	// 遍历整个图像，找到像素值为255的坐标
	for (int y = 0; y < mat.rows; ++y) {
		for (int x = 0; x < mat.cols; ++x) {
			if (mat.at<uchar>(y, x) == 255) {
				coordinates.push_back(cv::Point(x, y));  // 存储坐标
			}
		}
	}

	return coordinates;
}

cv::Point rotatePoint(const cv::Point& pt, const int degree, const cv::Size& imgSize)
{
	cv::Point rotatedPt = pt;
	int rows = imgSize.height;
	int cols = imgSize.width;

	if (degree == 90) {
		rotatedPt = cv::Point(rows - pt.y - 1, pt.x);  // 旋转90度
	}
	else if (degree == 180) {
		rotatedPt = cv::Point(cols - pt.x - 1, rows - pt.y - 1);  // 旋转180度
	}
	else if (degree == 270) {
		rotatedPt = cv::Point(pt.y, cols - pt.x - 1);  // 旋转270度
	}

	return rotatedPt;
}

int main() {
	//cv::Mat src = cv::imread("E:\\Image\\biaoding1.jpg");
	//// aruco::
	//cv::aruco::Dictionary dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
	//cv::Ptr<cv::aruco::Dictionary> dictionary = std::make_shared<cv::aruco::Dictionary>(dict);
	//// dict.writeDictionary("E:\\Image\\dict.jpg",);
	//  // cv::imshow("ff", src);
	//  // cv::waitKey();
 //Mat markerImage; 
 //dict.generateImageMarker(5,800,markerImage);
 // //  cv::aruco::drawCharucoDiamond(dict, 10, 1, 1, markerImage);
	//// std::vector<int> marker_ids;
	//// std::vector<std::vector<cv::Point2f>> marker_corners;
	//// cv::aruco::DetectorParameters parameters;
	//// parameters.refine_corners = true;
	//// cv::aruco::detectMarkers(src, dictionary, marker_corners, marker_ids);
	// cv::imshow("ff", markerImage);
	// cv::waitKey();
	cv::Mat src(480, 848, CV_8UC1, cv::Scalar::all(0));
	cv::Point p1(59, 303);
	src.at<uchar>(p1) = 255;
	cv::Mat rot90 = rotateImage(src, 90);
	cv::Point ro1 = rotatePoint(p1, 90, cv::Size(848, 480));
	std::vector<cv::Point> vec = find255Coordinates(rot90);

	std::cout << "old " << vec[0].x << "," << vec[0].y << std::endl;
	std::cout << "iii " << ro1.x << "," << ro1.y << std::endl;

	//cv::imshow("ff", src);
	//cv::waitKey();
	return 0;
}