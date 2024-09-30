#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Open "Road.mp4"
    VideoCapture cap;
    if(!cap.open("Road.mp4")) {
        cout << "no such video" << endl;
        return -1;
    }

    // Get fps and calculate delay
    int fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;
    int frame_limit = fps * 20; 
    int current_frame = 0;

    // Define the left and right roi rectangles
    Rect left_rect(200, 400, 400, 200);  // LT (200,400), RB (600,600)
    Rect right_rect(600, 400, 400, 200); // LT (600,400), RB (1000,600)

    Mat frame, gray, blurred;
    Mat left_roi, right_roi, left_canny, right_canny;
    Mat frame_line;

    vector<Vec2f> left_lines, right_lines;

    // 20초만 출력해야 함
    while (1) {
        cap >> frame;
        if (frame.empty()) {
            cout << "end of video" << endl;
            break;
        }
        current_frame++;

        // Convert the color frame to grayscale
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Apply blur to reduce noise
        blur(gray, blurred, Size(5, 5));  // 5x5 blur

        // Set left_roi and right_roi from the blurred image
        left_roi = blurred(left_rect);
        right_roi = blurred(right_rect);

        // Perform Canny edge detection
        Canny(left_roi, left_canny, 10, 60);    // low threshold: 10 , high threshold: 60
        Canny(right_roi, right_canny, 10, 60);

        // apply Hough Transform to find lines
        HoughLines(left_canny, left_lines, 1, CV_PI / 180, 60, 30, 60);
        HoughLines(right_canny, right_lines, 1, CV_PI / 180, 60, 120, 150);

        frame_line = frame.clone(); // frame to display with line

        // left roi
        for (int i = 0; i < left_lines.size(); i++) {
            float rho, theta, angle;
            rho = left_lines[i][0];
            theta = left_lines[i][1];
            angle = theta * 180 / CV_PI;    // find the angle by using theta

            if (angle > 30 && angle < 60) {  // filtering the angle
                Point p1, p2;
                float a, b, x0, y0;

                a = cos(theta);
                b = sin(theta);
                x0 = a * rho;
                y0 = b * rho;

                p1 = Point(cvRound(x0 + 1000 * (-b)) + left_rect.x, cvRound(y0 + 1000 * a) + left_rect.y);
                p2 = Point(cvRound(x0 - 1000 * (-b)) + left_rect.x, cvRound(y0 - 1000 * a) + left_rect.y);
                line(frame_line, p1, p2, Scalar(0, 0, 255), 3, 8);

                break;  // for draw only one line
            }
        }

        // right roi
        for (int i = 0; i < right_lines.size(); i++) {
            float rho, theta, angle;
            rho = right_lines[i][0];
            theta = right_lines[i][1];
            angle = theta * 180 / CV_PI;    // find the angle by using theta

            if (angle > 120 && angle < 150) {  // filtering the angle
                Point p1, p2;
                float a, b, x0, y0;

                a = cos(theta);
                b = sin(theta);
                x0 = a * rho;
                y0 = b * rho;

                p1 = Point(cvRound(x0 + 1000 * (-b)) + right_rect.x, cvRound(y0 + 1000 * a) + right_rect.y);
                p2 = Point(cvRound(x0 - 1000 * (-b)) + right_rect.x, cvRound(y0 - 1000 * a) + right_rect.y);
                line(frame_line, p1, p2, Scalar(0, 0, 255), 3, 8);

                break;  // for draw only one line
            }
        }   

        // Display
        namedWindow("Left canny");
        moveWindow("Left canny", 200, 0);
        imshow("Left canny", left_canny);

        namedWindow("Right canny");
        moveWindow("Right canny", 600, 0);
        imshow("Right canny", right_canny);

        namedWindow("Frame");
        moveWindow("Frame", 0, 200);
        imshow("Frame", frame_line);

        // Exit if the ESC key is pressed
        if (waitKey(delay) == 27) break;
        if (current_frame >= frame_limit) break;
    }

    return 0;
}
