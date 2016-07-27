#ifndef RECALL_RATE_H
#define RECALL_RATE_H

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/objdetect.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"

#include <vector>

extern std::string test_data_folder;
extern std::string test_info;

void evaluate_recall_rate(std::vector<cv::Rect> faces,std::vector<cv::Rect> faces_ans, cv::Mat *frame);
void evaluate_pos_neg_rate(std::vector<cv::Rect> faces,std::vector<cv::Rect> faces_ans, cv::Mat *frame);

#endif
