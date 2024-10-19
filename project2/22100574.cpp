#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap;

    if (cap.open("Project2_video.mp4") == 0) {
        cout << "no such video!" <<endl;
        return -1;
    }

    int fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    // for lane departure
    Mat frame;
    Mat canny_left, canny_right, canny_temp;
    Rect rect_left(190, 270, 160, 200);
    Rect rect_right(420, 270, 160, 200);
    Rect rect_temp(70, 330, 180, 120);
    vector<Vec2f> lines1, lines2, lines3;
    int lane_count = 0;

    // for start moving
    Mat roi_car, background, foregroundMask;
    int background_cnt = 10;
    int display = 0;
    int display_cnt = 0;
    int flag = 0;
    int nonzero_cnt;
    Rect rect_car(290, 230, 140, 80);

    // set first background
    cap >> frame;
    background = frame(rect_car);
    cvtColor(background, background, COLOR_BGR2GRAY);
    for (int c = 2; c <= background_cnt; c++) {
        cap >> frame;
        roi_car = frame(rect_car);
        cvtColor(roi_car, roi_car, COLOR_BGR2GRAY);
        add(roi_car / c, background*(c-1)/c, background);
    }

    while(1) {
        cap >> frame;
        if (frame.empty()) {
            cout << " end of video" << endl;
            break;
        }

        // 1. Lane Detection code
        canny_left = frame(rect_left);
        cvtColor(canny_left, canny_left, COLOR_BGR2GRAY);
        blur(canny_left, canny_left, Size(5, 5));
        Canny(canny_left, canny_left, 10, 100, 3);
        HoughLines(canny_left, lines1, 1, CV_PI/180, 60, 0, 0, CV_PI / 6, CV_PI / 18 * 5);

        canny_right = frame(rect_right);
        cvtColor(canny_right, canny_right, COLOR_BGR2GRAY);
        blur(canny_right, canny_right, Size(5, 5));
        Canny(canny_right, canny_right, 10, 80, 3);
        HoughLines(canny_right, lines2, 1, CV_PI/180, 60, 0, 0, CV_PI / 3 * 2, CV_PI / 6 * 5);

        canny_temp = frame(rect_temp);
        cvtColor(canny_temp, canny_temp, COLOR_BGR2GRAY);
        blur(canny_temp, canny_temp, Size(5, 5));
        Canny(canny_temp, canny_temp, 10, 100, 3);
        HoughLines(canny_temp, lines3, 1, CV_PI/180, 60, 0, 0, CV_PI / 3, (CV_PI / 18) * 7);

        if (lane_count > 10) putText(frame, format("Lane departure!"), Point(10, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
        if (!lines1.size() && !lines2.size() && lines3.size()) lane_count++;
        if (lines1.size() && lines2.size()) lane_count = 0;

        // 2. start moving code
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