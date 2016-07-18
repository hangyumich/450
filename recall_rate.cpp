#include "recall_rate.h"
#include "utility.h"

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace cv;

string face_cascade_name = "/home/ubuntu/face_cascade_example/profile_face_model/cascade.xml";

CascadeClassifier face_cascade;

std::vector<int> pos_answer_num;
std::vector<int> pos_detected_num;
std::vector<int> total_answer_num;
std::vector<int> total_detected_num;


int main( int argc, char** argv )
{	
	std::vector<String> image;
	std::vector<int> actual_face_num;
	std::vector<int> detected_face_num;

	if (argc == 1){
		//Scan the folder
		image=getImageNames();
	}
	else if (argc == 2){
		//Detect one file
		image.push_back(argv[1]);
	}
    else
    {
	   printf("Wrong command.");
       return -1;
    }

	//Load Trained Model   
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
       
	for (int num = 0; num < image.size(); num++){//per image in the folder	
		//Load Image
		Mat frame;
		Mat frame_gray;
		frame = imread( image[num], 1 );
		cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
		equalizeHist( frame_gray, frame_gray );
		
		if ( frame.data ){
			//Detect faces
			std::vector<Rect> faces;    
			//std::vector<int> fakeLevels;
			//std::vector<double> fakeWeights;
			//bool b;
			//face_cascade.detectMultiScaleNoGrouping( frame_gray, faces, fakeLevels, fakeWeights, 1.1, 0, Size(200, 200),b );
			face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(200, 200) );		
			total_detected_num.push_back(faces.size());
			
			//Get answers from txt file
			std::vector<Rect> faces_ans;
			faces_ans = ReadRectInfo( test_data_folder, test_info, image[num]);
			total_answer_num.push_back(faces_ans.size());
					
			//Overlap Analysis
			evaluate_recall_rate(faces, faces_ans, &frame);
			evaluate_pos_neg_rate(faces_ans, faces, &frame);
			
			//Display the window
			// namedWindow(image[num], WINDOW_AUTOSIZE );
			imwrite(image[num], frame );  
		}
	}
	
	//Print test result
	cout<< "\n****** Test Result ******\n";
	cout<<"name\tdetected\tface number\tpositive\tnegative\n";
	for (int num = 0; num < image.size(); num++){//per image in the folder	
		cout << image[num] << "\t" << pos_answer_num[num] << "\t" << total_answer_num[num]<<"\t";
		cout << pos_detected_num[num] << "\t" << total_detected_num[num]-pos_detected_num[num] << "\n";
	}
	
	cout<<"*Sum*:\t"<< vectorsum(pos_answer_num) << "\t" << vectorsum(total_answer_num)<<"\t";
	cout << vectorsum(pos_detected_num) << "\t" << vectorsum(total_detected_num)-vectorsum(pos_detected_num) << "\n";

    cout<<"\n\n**Recall Rate**:"<< vectorsum(pos_answer_num)*100/vectorsum(total_answer_num) <<"%\n\n";

	waitKey(0);
	return 0;
}


/* evaluate_recall_rate
 * If the rectangle in the answer is found, mark in green. If not, mark
 * in red. "Found" means larger than 75% overlapped by a detected rect.
 * 
 * Effects: 
 * 1. add an element to pos_answer_num
 * 2. add rects to frame
 */
void evaluate_recall_rate(std::vector<Rect> faces,std::vector<Rect> faces_ans, Mat *frame){		
		int score = 0;
		for( size_t i = 0; i < faces_ans.size(); i++ ){//per face in answer
			if (overlap_bool(faces_ans[i], faces, 75)) {
				rectangle(*frame, faces_ans[i], Scalar(0,255,0),2,0);
				score++;
			} else rectangle(*frame, faces_ans[i], Scalar(0,0,255),2,0);
		}
		pos_answer_num.push_back(score);	
}

void evaluate_pos_neg_rate(std::vector<Rect> faces,std::vector<Rect> faces_ans, Mat *frame){
		int score = 0;
		for( size_t i = 0; i < faces_ans.size(); i++ ){//per face in answer
			if (overlap_bool(faces_ans[i], faces, 50)) {
				rectangle(*frame, faces_ans[i], 255,1,0);
				score++;
			} else rectangle(*frame, faces_ans[i], 0,1,0);
		}
		pos_detected_num.push_back(score);	
}


