#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat frame;

    VideoCapture cap;

    if (cap.open("Project2_video.mp4") == 0) {
        cout << "no such video!" <<endl;
        return -1;
    }

    int fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    Mat roi_car;
    Rect rect_car(290, 230, 140, 80);

    // set first background
    Mat background;
    int background_cnt = 10;
    cap >> frame;
    background = frame(rect_car);
    cvtColor(background, background, COLOR_BGR2GRAY);
    for (int c = 2; c <= background_cnt; c++) {
        cap >> frame;
        roi_car = frame(rect_car);
        cvtColor(roi_car, roi_car, COLOR_BGR2GRAY);
        add(roi_car / c, background*(c-1)/c, background);
    }

    Mat foregroundMask;
    int display = 0;
    int display_cnt = 0;
    int flag = 0;
    int nonzero_cnt;

    while(1) {
        cap >> frame;
        if (frame.empty()) {
            cout << " end of video" << endl;
            break;
        }

        roi_car = frame(rect_car);
        cvtColor(roi_car, roi_car, COLOR_BGR2GRAY);

        absdiff(background, roi_car, foregroundMask);
        threshold(foregroundMask, foregroundMask, 50, 255, THRESH_BINARY);
        nonzero_cnt = countNonZero(foregroundMask);
        
        if ((!flag) && nonzero_cnt > 500) {
            display = 1;
            flag = 1;
        }

        if (flag && nonzero_cnt < 20) {
            flag = 0;
        }

        if (display) {
            putText(frame, format("Start Moving!"), Point(10, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
            display_cnt++;
        }
        if (display_cnt > (fps*5)) {    // display "Start Moving!" text only 5 sec.
            display = 0;
            display_cnt = 0;
        }

        // update background before next iterator
        add(roi_car / background_cnt, background*(background_cnt-1)/background_cnt, background);

        imshow("frame", frame);
        if (waitKey(delay) == 27) break;
    }


    return 0;
}