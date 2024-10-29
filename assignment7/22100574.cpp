#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main () {
    VideoCapture cap;
    Mat frame;

    if (cap.open("Faces.mp4") == 0) {
        cout << "no such video!" <<endl;
        return -1;
    }

    int fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    while(1) {
        cap >> frame;
        
        if(frame.empty()) {
            cout << "end of video!" << endl;
            break;
        }

        int key = waitKey(delay);

        if(key == 'n') {
            // detect the nearest face
        } else if (key == 'f') {
            // detect the farthest face
        } else if (key == 'm') {
            // detect the face at the middel range
        } else if (key == 'r') {
            // reset program
        } else if (key == 't') {
            // track only the region of face you chose
            // another window를 사용해서 face region만 보여주기 ("tracking" window name)
            // 다시 t를 누르면 destroy window
            // window 지정 전에 t를 누르면 -> "Detect before tracking" message 출력
        }
    }
    
    return 0;
}