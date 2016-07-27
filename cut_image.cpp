#include "utility.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;
using namespace cv;


extern std::string test_data_folder;
extern std::string test_info;

string face_cascade_name = "/home/ubuntu/face_cascade_example/profile_face_model/cascade.xml";
CascadeClassifier face_cascade;

bool testOverlap(Rect&, Rect&, int);
void testNextRect(std::vector<Rect>&, Rect&, std::vector<int>&);
void testAllRect(std::vector<Rect>&, std::vector<Rect>&, std::vector<int>&);
void CutRect(std::string image_name, std::vector<Rect>&, Mat*);

unordered_map<string, vector<Rect>> images_info;

int main( int argc, char** argv )
{
    std::vector<string> image;

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
	
    ReadImagesInfo(test_data_folder + test_info, images_info);
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };

    // cout <<  "image size:" << image.size() << endl;
	for (int num = 0; num < image.size(); num++){//per image in the folder  
        cout << "processing image "<< image[num] << endl;
        Mat frame;
        Mat frame_gray;
        std::vector<Rect> faces;
            
        //Load Trained Model   
        //Load Image
        frame = imread( image[num], 1 );
    	//Preprocess
        cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
        equalizeHist( frame_gray, frame_gray );
        //Detect faces
        face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(200, 200) );     
        // cout << "after detect face." << endl;
        //combine rectangles------added code
        std::vector<Rect> ResultFaces = {};
        if (!faces.empty())
        {
            ResultFaces.push_back(faces[0]); 
            std::vector<int> weight;
            weight.push_back(1);
            testAllRect(ResultFaces, faces, weight);
            // cout << "after test all rect" << endl;
        }
        //Cut rectangles
        CutRect(image[num] ,faces, &frame); 
    }
	return 0;
}

void CutRect(std::string image_path, std::vector<Rect>& ResultFaces, Mat* ptr) {
    string image_name = image_path.substr(image_path.find_last_of("/")+1);
    image_name = image_name.substr(0, image_name.find_last_of("."));
    // cout <<  "resultfaces size: " << ResultFaces.size() << endl;
    for( size_t i = 0; i < ResultFaces.size(); i++ )
    {
        Point top_left( ResultFaces[i].x, ResultFaces[i].y);
        Point bottom_right( ResultFaces[i].x + ResultFaces[i].width, ResultFaces[i].y + ResultFaces[i].height );
        //make sure rect inside original image 
        if((ResultFaces[i].x > 0) && (ResultFaces[i].y > 0) && 
            ((ResultFaces[i].width + ResultFaces[i].x) < (*ptr).size().width) &&
            ((ResultFaces[i].height + ResultFaces[i].y) < (*ptr).size().height)) {
            //cut image
            cv::Mat croppedFaceImage;
            croppedFaceImage = (*ptr)(ResultFaces[i]).clone();
            std::string path;
            if (overlap_bool(ResultFaces[i], images_info[image_name], 50, 0))
                path = "positive/";
            else
                path = "negative/";
            std::string name = image_name + "_" + to_string(i) + ".jpg";
            // cout << "writing to " << path << name << endl;
            imwrite(path+name, croppedFaceImage);
        }
    }
}

//move the true rect up, down, left, right
// void moveRect(std::vector<Rect>& ResultFaces) {
//     size_t orisize = ResultFaces.size();
//     for(size_t i = 0; i < orisize; i++) {
//         //if(....) {
//             Rect move = ResultFaces[i];
//             move.x = ResultFaces[i].x + ResultFaces[i].width * rand() % 100 / 2000.;
//             ResultFaces.push_back(move);
//             move = ResultFaces[i];
//             move.x = ResultFaces[i].x - ResultFaces[i].width * rand() % 100 / 2000.;
//             ResultFaces.push_back(move);
//             move = ResultFaces[i];
//             move.y = ResultFaces[i].y - ResultFaces[i].height * rand() % 100 / 2000.;
//             ResultFaces.push_back(move);
//             move = ResultFaces[i];
//             move.y = ResultFaces[i].y + ResultFaces[i].height * rand() % 100 / 2000.;
//             ResultFaces.push_back(move);
//          //}
//     }
    
// }

void testAllRect(std::vector<Rect>&  ResultFaces, std::vector<Rect>& faces, std::vector<int>& weight) {
    for(size_t i = 1; i < faces.size(); i++) {
        testNextRect(ResultFaces, faces[i], weight);
    }
}

///////////////////////////////////////////////
//  a-----------------b
//  |                 |
//  d-----------------c
///////////////////////////////////////////////
//test overlap; if overlap, adjust overlapped rect
bool testOverlap(Rect& r1, Rect& r2, int w) {

    int ax = std::max(r1.x, r2.x);
    int ay = std::max(r1.y, r2.y);
    int cx = std::min(r1.x + r1.width, r2.x + r2.width);
    int cy = std::min(r1.y + r1.height, r2.y + r2.height);
    int minArea = std::min(r1.width * r1.height, r2.width * r2.height);

    if((ax <= cx) && (ay <= cy) && (((cy-ay)*(cx-ax)) >= (0.95*minArea))){
        r1.width = (r1.width * w + r2.width)/(w + 1);
        r1.height = (r1.height * w + r2.height)/(w + 1);
        r1.x = (r1.x * w + r2.x)/(w + 1);
        r1.y = (r1.y * w + r2.y)/(w + 1);
        return true;
    }else {
        return false;
    }

}

// if not overlap, add to ResultFaces;
void testNextRect(std::vector<Rect>& ResultFaces, Rect& r1, std::vector<int>& weight) {
    for(size_t i = 0; i < ResultFaces.size(); i++) {
        if(testOverlap(ResultFaces[i], r1, weight[i])) {
            weight[i]++;
            return;
        }
    }
    ResultFaces.push_back(r1);
    weight.push_back(1);
}