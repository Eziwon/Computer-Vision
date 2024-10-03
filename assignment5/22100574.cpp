#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat finger_print, adaptive_1, adaptive;

    // Read all images as grayscale
    finger_print = imread("finger_print.png", 0);
    adaptive = imread("adaptive.png", 0);
    adaptive_1 = imread("adaptive_1.jpg", 0);

    // for finger_print image set printer region to 0 and background region to 255
    // Use Global thresholding
    Mat finger_result;
    int thresh_T, th, low_cnt, high_cnt, low_sum, high_sum;
    thresh_T = 100; // Initial threshold
    th = 10;
    low_cnt = high_cnt = low_sum = high_sum = 0;

    while(1) {
        // calculate pixel values
        for (int j = 0; j < finger_print.rows; j++) {
            for (int i = 0; i < finger_print.cols; i++) {
                if (finger_print.at<uchar>(j, i) < thresh_T) {
                    low_sum += finger_print.at<uchar>(j, i);
                    low_cnt++;
                } else {
                    high_sum += finger_print.at<uchar>(j, i);
                    high_cnt++;
                }
            }
        }
        if (abs(thresh_T - (low_sum/low_cnt + high_sum/high_cnt) / 2.0f) < th) break;
        else {  // set new threshold
            thresh_T = (low_sum/low_cnt + high_sum/high_cnt) / 2.0f;
            low_cnt = high_cnt = low_sum = high_sum = 0;
        }
    }
    threshold(finger_print, finger_result, thresh_T, 255, THRESH_BINARY);

    // For ‘adatpve_1.jpg’, and ‘adaptive.png’, set character region to 0 and background region to 255
    // Use adaptive threshold
    Mat adaptive_1_result;
    adaptiveThreshold(adaptive_1, adaptive_1_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 25, 10);

    Mat adaptive_result;
    adaptiveThreshold(adaptive, adaptive_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 10);

    // Display
    namedWindow("finger print");
    moveWindow("finger print", 200, 0);
    imshow("finger print", finger_result);

    namedWindow("adaptive 1 result");
    moveWindow("adaptive 1 result", 400, 0);
    imshow("adaptive 1 result", adaptive_1_result);

    namedWindow("adaptive result");
    moveWindow("adaptive result", 200, 250);
    imshow("adaptive result", adaptive_result);

    waitKey(0);

    return 0;
}