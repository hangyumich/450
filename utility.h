#ifndef UTILITY_H
#define UTILITY_H

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/objdetect.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"

#include <iosfwd>
#include <vector>
#include <unordered_map>

extern std::string test_data_folder;
extern std::string test_info;

int vectorsum(std::vector<int> v);
void ReadImagesInfo(std::string file_path, std::unordered_map<std::string, std::vector<cv::Rect>>&);
void draw_rects(cv::Mat *frame, std::vector<cv::Rect> faces, int color, int thickness, int lineType);
bool overlap_bool(cv::Rect rectA, std::vector<cv::Rect> answer, int threshold, bool percentage_over_itself);
std::vector<std::string> getImageNames (std::string filename);

#endif
