#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat frame;
    int fps, delay, total_frame_cnt;
    int current_frame_index;
    VideoCapture cap;

    if (cap.open("background.mp4") == 0) {
        cout << "no such file!" << endl;
        waitKey(0);
        return -1;
    }

    fps = cap.get(CAP_PROP_FPS);
    delay = 1000 / fps;
    total_frame_cnt = cap.get(CAP_PROP_FRAME_COUNT);    // Get total number of frame

    int frame_limit = fps * 3;  // Number of frames in 3 seconds

    while(1) {
        cap >> frame;
        if (frame.empty()) {
            cout << "end of video" << endl;
            break;
        }

        // Print out the number of the current frame and the total number of frames
        current_frame_index = cap.get(CAP_PROP_POS_FRAMES);
        cout << "frames:" << current_frame_index << "/" << total_frame_cnt << endl;

        imshow("video", frame);
        waitKey(delay);

        if (current_frame_index >= frame_limit) break;
    }

    return 0;
}