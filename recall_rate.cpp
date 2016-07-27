#include "recall_rate.h"
#include "utility.h"

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>

using namespace std;
using namespace cv;

string face_cascade_name = "/home/ubuntu/face_cascade_example/profile_face_model/cascade.xml";
//string face_cascade_name = "cascade.xml"; //zwx

CascadeClassifier face_cascade;

std::vector<int> pos_answer_num;
std::vector<int> pos_detected_num;
std::vector<int> total_answer_num;
std::vector<int> total_detected_num;
std::vector<long double> time_elapsed;
unordered_map<string, vector<Rect>> images_info;

int main( int argc, char** argv )
{	
	std::vector<std::string> image;
	std::vector<int> actual_face_num;
	std::vector<int> detected_face_num;
	int show_window = 0;

	if (argc == 1){
		//Scan the folder
		image=getImageNames();
	}
	else if (argc == 2){
		//Detect one file
		image.push_back(argv[1]);
		show_window = 1;
	}
    else
    {
	   printf("Wrong command.");
       return -1;
    }

	//Load Trained Model   
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    
	ReadImagesInfo(test_data_folder + test_info, images_info);

	for (int num = 0; num < image.size(); num++){//per image in the folder	
		//Load Image
		Mat frame;
		Mat frame_gray;
		clock_t t1,t2;
		
		frame = imread( image[num], 1 );
		cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
		equalizeHist( frame_gray, frame_gray );
			
		if ( frame.data ){
			//Detect faces
			std::vector<Rect> faces;    
			t1 = clock();
			face_cascade.detectMultiScale( frame_gray, faces, 1.2, 2, 0, frame.size()/5 , frame.size() );
			total_detected_num.push_back(faces.size());
			t2 = clock();
			time_elapsed.push_back(((long double)t2-(long double)t1)/CLOCKS_PER_SEC);
			
			//Get answers
       	 	string image_name = image[num].substr(image[num].find_last_of("/")+1);
        	image_name = image_name.substr(0, image_name.find_last_of("."));
			total_answer_num.push_back(images_info[image_name].size());

			//Overlap Analysis
			evaluate_recall_rate(faces, images_info[image_name], &frame);
			evaluate_pos_neg_rate(images_info[image_name], faces, &frame);
			if (pos_answer_num[num]<total_answer_num[num]) imwrite("Failed Tests/"+image_name+".jpg", frame );  
			
			//Display the window
			if (show_window) {
				namedWindow(image_name, WINDOW_AUTOSIZE );
				imshow(image_name, frame );  
			}
		}
	}
	
	//Print test result
	cout<< "\n****** Test Result ******\n";
	cout<<"name\tdetected\tface number\tpositive\tnegative\n";
	for (int num = 0; num < image.size(); num++){//per image in the folder	
		cout << image[num] << "\t" << pos_answer_num[num] << "\t" << total_answer_num[num]<<"\t";
		cout << pos_detected_num[num] << "\t" << total_detected_num[num]-pos_detected_num[num] <<"\t"<<time_elapsed[num]<<"\n";
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
			if (overlap_bool(faces_ans[i], faces, 75, 1)) {
				rectangle(*frame, faces_ans[i], Scalar(0,255,0),2,0);//green
				score++;
			} else rectangle(*frame, faces_ans[i], Scalar(0,0,255),2,0);//red
		}
		pos_answer_num.push_back(score);	
}

void evaluate_pos_neg_rate(std::vector<Rect> faces,std::vector<Rect> faces_ans, Mat *frame){
		int score = 0;
		for( size_t i = 0; i < faces_ans.size(); i++ ){//per face in answer
			if (overlap_bool(faces_ans[i], faces, 50, 0)) {
				rectangle(*frame, faces_ans[i], 255,1,0);//blue
				score++;
			} else rectangle(*frame, faces_ans[i], 0,1,0);//black
		}
		pos_detected_num.push_back(score);	
}


