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

    // Define the left and right roi rectangles
    Rect left_rect(200, 400, 400, 200);  // LT (200,400), RB (600,600)
    Rect right_rect(600, 400, 400, 200); // LT (600,400), RB (1000,600)

    Mat frame, gray, blurred;
    Mat left_roi, right_roi, left_canny, right_canny;

    while (1) {
        cap >> frame;
        if (frame.empty()) {
            cout << "end of video" << endl;
            break;
        }

        // Convert the frame to grayscale
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Apply blur to reduce noise
        blur(gray, blurred, Size(5, 5));  // 5x5 blur

        // Set left and right roi from the blurred image
        left_roi = blurred(left_rect);
        right_roi = blurred(right_rect);

        // Perform Canny edge detection
        Canny(left_roi, left_canny, 10, 60);
        Canny(right_roi, right_canny, 10, 60);

        // Display the Canny edges
        namedWindow("Left canny");
        moveWindow("Left canny", 200, 0);
        imshow("Left canny", left_canny);

        namedWindow("Right canny");
        moveWindow("Right canny", 600, 0);
        imshow("Right canny", right_canny);

        // Exit if the ESC key is pressed
        if (waitKey(delay) == 27) break;
    }

    return 0;
}
