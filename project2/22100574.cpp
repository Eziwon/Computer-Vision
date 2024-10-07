#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap("Project2_video.mp4");

    int fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    Mat background, frame, gray, result;
    while(true) {

        waitKey(delay);
    }
    
    return 0;
}