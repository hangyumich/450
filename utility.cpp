#include "utility.h"

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace cv;

string test_data_folder = "/home/ubuntu/get_face_info/";
//string test_data_folder = "/media/zwx/a03a94ae-d453-48cc-9a57-5990509f8d71/wenxuan/Documents/july21/";
string test_info = "pos_info_left.txt";

int vectorsum(vector<int> v){
    int sum=0;
    for(vector<int>::iterator it = v.begin(); it != v.end(); ++it){    
        sum += *it;
    }
    return sum;
}

void ReadImagesInfo(string file_path, unordered_map<string, vector<Rect>>& images_info){
    //Read in real positions
    ifstream filestream;
    filestream.open(file_path.c_str());
    if ( !filestream.is_open()){
        cout << "open failed" << endl;
        cout << "in readinfo, " << file_path << endl;
        exit(1);
    }
    string line;
    string image_path;
    int face_num;
    int x,y,w,h;
    while (getline(filestream, line)) {
        vector<Rect> image_rects;
        istringstream iss(line);
        iss >> image_path;
        iss >> face_num;
        string image_name = image_path.substr(image_path.find_last_of("/")+1);
        image_name = image_name.substr(0, image_name.find_last_of("."));
        for (int i = 0; i < face_num; i++ ){
                iss >> x >> y >> w >> h;
                image_rects.push_back(Rect(x,y,w,h));
        }
        images_info[image_name] = image_rects;
    }
    filestream.close();
}



/* overlap_bool
 * Compare rectangle "rectA" with every element in rectangle vector "answer".
 *  If the overlap area over the original area of "rectA" is larger than 
 * "threshold" in percentage, return 1, return 0 otherwise.
 * 
 * percentage_over_itself = 1: percentage is calculated with the area of
 * rectA as denominator(for recall rate). 
 * percentage_over_itself = 0: the area of the answer is the denominator
 * (for pos-neg labels).
 */
bool overlap_bool(Rect rectA, vector<Rect> answer, int threshold, bool percentage_over_itself ){
    int x1,y1,x2,y2,overlap;
    for (int j = 0; j < answer.size(); j++ ){//per face detected
        x1 = max(rectA.x,answer[j].x);
        x2 = min(rectA.x+rectA.width,answer[j].x+answer[j].width);
        y1 = max(rectA.y,answer[j].y);
        y2 = min(rectA.y+rectA.height,answer[j].y+answer[j].height);
        if ((x2>x1) && (y2>y1)) { 
            if (percentage_over_itself){
                overlap = 100*(x2-x1)*(y2-y1)/(rectA.width * rectA.height);//for recall rate
            } else {
                overlap = 100*(x2-x1)*(y2-y1)/(answer[j].width * answer[j].height);//for pos/neg labels
            }
            if (overlap > threshold) return 1;
            ////used for verification
            //cout << "x1=" << x1 <<  " y1=" << y1 << " x2=" << x2 << " y2=" << y2 <<"\n";
            //cout << faces_ans[i].width << " " << faces_ans[i].height << "\n";
            //cout << "overlap=" << overlap <<"\n"; 
            //putText(frame, "Face" , Point(x1,y1),1, 1, 255);
            //Point top_left( x1, y1);
            //Point bottom_right( x2, y2 );
            //rectangle(frame, top_left, bottom_right, 255, 1, 0);
        }
    }
    return 0;
}

// void draw_rects(Mat *frame, vector<Rect> faces, int color, int thickness, int lineType){
//     for( size_t i = 0; i < faces.size(); i++ )
//         {
//             Point top_left( faces[i].x, faces[i].y);
//             Point bottom_right( faces[i].x + faces[i].width, faces[i].y + faces[i].height );
//             rectangle(*frame, top_left, bottom_right, color, thickness, lineType);
//             //cout << faces[i].x <<" "<<faces[i].y<<" "<< faces[i].x+faces[i].width<<" "<< faces[i].y+faces[i].height<<"\n"; 
//         }
// }
   
std::vector<std::string> getImageNames (){
    std::vector<std::string> Ans;

    ifstream filestream;
    string filename;
    filename = test_data_folder + test_info;
    filestream.open(filename.c_str());
    if ( !filestream.is_open()){
        cout << "open failed" << endl;
        cout << "in getImageNames, " << filename << endl;
        exit(1);
    }
    string line;
    string word;
    while (getline(filestream, line)) {
        istringstream iss(line);
        iss >> word;    
        ifstream f(word);
        if (f.good()) Ans.push_back(word);//if the file exists
    }
    filestream.close();
    return Ans;
}
