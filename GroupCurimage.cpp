#include "Utility.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

using namespace std;
using namespace cv;


extern std::string test_data_folder;
extern std::string test_info;

String face_cascade_name = "/home/ubuntu/face_cascade_example/profile_face_model/cascade.xml";
CascadeClassifier face_cascade;

void detectAndDisplay( Mat frame );
bool testOverlap(Rect&, Rect&, int);
void testNextRect(std::vector<Rect>&, Rect&, std::vector<int>&);
// void moveRect(std::vector<Rect>&);
void testAllRect(std::vector<Rect>&, std::vector<Rect>&, std::vector<int>&);
void CutRect(std::string image_name, std::vector<Rect>&, Mat*);

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

int main( int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
	
	
    Mat frame;
    Mat frame_gray;
    std::vector<Rect> faces;
        
    //Load Trained Model   
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    //Load Image
    frame = imread( argv[1], 1 );
	//Preprocess
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    //Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0, Size(200, 200) );
    
    //combine rectangles------added code
    std::vector<Rect> ResultFaces;
    ResultFaces.push_back(faces[0]); 
    std::vector<int> weight;
    weight.push_back(1);
    testAllRect(ResultFaces, faces, weight);
    // moveRect(ResultFaces);

    //Cut rectangles
    CutRect(argv[1] ,ResultFaces, &frame);
    
    //Display the window
    // namedWindow("Display Image", WINDOW_AUTOSIZE );
    // imshow( "Display Image", frame );  

	waitKey(0);
	return 0;
}

void CutRect(std::string image_name, std::vector<Rect>& ResultFaces, Mat* ptr) {
    std::vector<Rect> faces_ans;
    faces_ans = ReadRectInfo(test_data_folder, test_info, image_name);

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
            if (overlap_bool(ResultFaces[i], faces_ans, 30))
                path = "positive/";
            else
                path = "negative/";
            std::string name = "croppedimage_" + patch::to_string(i) + ".jpg";
            cout << "writing to " << path << name << endl;
            imwrite(name, croppedFaceImage);
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

    if((ax <= cx) && (ay <= cy) && (((cy-ay)*(cx-ax)) >= (0.8*minArea))){
        r1.width = (r1.width * w + r2.width)/(w + 1);
        r1.height = (r1.height * w + r2.height)/(w + 1);
        r1.x = (r1.x * w + r2.x)/(w + 1);
        r1.y = (r1.y * w + r2.y)/(w + 1);
        return true;
    }else {
        return false;
    }

}

//if not overlap, add to ResultFaces; 
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