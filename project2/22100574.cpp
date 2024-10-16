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

    Rect rect_left(180, 250, 120, 200);
    Rect rect_right(400, 250, 120, 200);
    Mat canny_left, canny_right;

    vector<Vec2f> lines1, lines2;
    float rho, theta, a, b, x0, y0, total_rho, total_theta;
    float rho1, theta1, a1, b1, x1, y1, total_rho1, total_theta1;
    Point p1, p2;
    Point p3, p4;
    int lane_count = 0;

    while(1) {
        cap >> frame;
        if (frame.empty()) {
            cout << " end of video" << endl;
            break;
        }

        canny_left = frame(rect_left);
        cvtColor(canny_left, canny_left, COLOR_BGR2GRAY);
        blur(canny_left, canny_left, Size(5, 5));
        Canny(canny_left, canny_left, 10, 100, 3);

        canny_right = frame(rect_right);
        cvtColor(canny_right, canny_right, COLOR_BGR2GRAY);
        blur(canny_right, canny_right, Size(5, 5));
        Canny(canny_right, canny_right, 10, 100, 3);

        HoughLines(canny_left, lines1, 1, CV_PI/180, 60, 0, 0, CV_PI / 6, CV_PI / 3);
        HoughLines(canny_right, lines2, 1, CV_PI/180, 60, 0, 0, CV_PI / 3 * 2, CV_PI / 6 * 5);
        
        if (!lines1.size() && !lines2.size()) {
            lane_count++;
        }
        if (lane_count > 10) putText(frame, format("Lane departure!"), Point(10, 10), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 4);
        if (lines1.size() && lines2.size()) lane_count = 0;

        // if (lines1.size()) {
        //     total_rho = 0;
        //     total_theta = 0;
        //     for (int i = 0; i < lines1.size(); i++) {
        //         rho = lines1[i][0];
        //         theta = lines1[i][1];

        //         total_rho += rho;
        //         total_theta += theta;
        //     }

        //     total_rho /= lines1.size();
        //     total_theta /= lines1.size();

        //     a = cos(total_theta);
        //     b = sin(total_theta);
        //     x0 = a * total_rho;
        //     y0 = b * total_rho;

        //     p1 = Point(cvRound(x0 + 1000 * (-b)) + 180, cvRound(y0 + 1000 * a) + 250);
        //     p2 = Point(cvRound(x0 - 1000 * (-b)) + 180, cvRound(y0 - 1000 * a) + 250);
        //     line(frame, p1, p2, Scalar(0, 0, 255), 3, 8);
        // }

        // if (lines2.size()) {
        //     total_rho1 = 0;
        //     total_theta1 = 0;
        //     for (int i = 0; i < lines2.size(); i++) {
        //         rho1 = lines2[i][0];
        //         theta1 = lines2[i][1];

        //         total_rho1 += rho1;
        //         total_theta1 += theta1;
        //     }

        //     total_rho1 /= lines2.size();
        //     total_theta1 /= lines2.size();

        //     a1 = cos(total_theta1);
        //     b1 = sin(total_theta1);
        //     x1 = a1 * total_rho1;
        //     y1 = b1 * total_rho1;

        //     p3 = Point(cvRound(x1 + 1000 * (-b1)) + 400, cvRound(y1 + 1000 * a1) + 250);
        //     p4 = Point(cvRound(x1 - 1000 * (-b1)) + 400, cvRound(y1 - 1000 * a1) + 250);
        //     line(frame, p3, p4, Scalar(0, 0, 255), 3, 8);
        // }

        imshow("canny_right", canny_right);
        imshow("frame", frame);

        if (waitKey(delay) == 27) break;
    }

    return 0;
}