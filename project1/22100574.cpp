#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int checkKey(int key);                      // check the type of entered key
void applyNegative(Mat frame);              // Apply Negative Transformation to frame
void applyGamma(Mat frame);                 // Apply Gamma Transformation to frame
void applyHistEqual(Mat frame);             // Apply Histogram Equalization to frame
void applyColorSlicing(Mat frame);          // Apply Color Slicing to frame
void applyColorConversion(Mat frame);       // Apply Color Conversion to frame
void applyAverageFiltering(Mat frame);      // Apply Average Filtering to frame
void applySharpening(Mat frame);            // Apply Sharpening to frame
void applyWhiteBalancing(Mat frame);        // Apply White Balancing to frame

int main() {
    // Open the "video.mp4"
    VideoCapture cap;
    if (!cap.open("video.mp4")) {
        cout << "no such video" << endl;
        return -1;
    }

    // Get fps and delay for display the video
    int fps, delay;
    fps = cap.get(CAP_PROP_FPS);
    delay = 1000 / fps;

    // Display the video
    Mat frame;
    int key;
    int type = 0;
    while (1) {
        cap >> frame;
        if (frame.empty()) {
            cout << "end of video" << endl;
            break;
        }

        key = waitKey(delay);
        if (key == 27) break;   // if entered ESC, finish the video.

        else if (key == 'r') {  // reset the image
            type = 0;
        }

        else if (key != -1) {   // When key is pressed, 
            int newType = checkKey(key);    // check the type of key
            if (newType != type && newType != -1) {   // 지금이랑 다른 type이면서, 유용한 key를 누른 경우
                type = newType;
            }
        }

        switch (type) {
            case 0: imshow("video", frame); break;
            case 'n': applyNegative(frame); break;
            case 'g': applyGamma(frame); break;
            case 'h': applyHistEqual(frame); break;
            case 's': applyColorSlicing(frame); break;
            case 'c': applyColorConversion(frame); break;
            case 'a': applyAverageFiltering(frame); break;
            case 'u': applySharpening(frame); break;
            case 'w': applyWhiteBalancing(frame); break;
            default: break;
        }
    }

    return 0;
}

int checkKey(int key) {
    if (key == 'n' || key == 'g' || key == 'h' || key == 's' || key == 'c' || key == 'a' || key == 'u' || key == 'w' || key == 'r') {
        return key;
    } else return -1;
}

void applyNegative(Mat frame) {
    Mat frame_HSV, negative_frame;
    vector<Mat> hsv_channels(3);

    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);  // Convert BGR to HSV
    split(frame_HSV, hsv_channels);     // split multiple channel to single channels

    // Negative Transformation of I mat
    for (int j = 0; j < frame.rows; j++) {
        for (int i = 0; i < frame.cols; i++) {
            hsv_channels[2].at<uchar>(j, i) = 255 - hsv_channels[2].at<uchar>(j, i);
        }
    }

    merge(hsv_channels, negative_frame);    // merge set of Mats to multiple channels
    cvtColor(negative_frame, negative_frame, COLOR_HSV2BGR);    // Convert HSV to BGR

    imshow("video", negative_frame);    // display negative transformed frame
}

void applyGamma(Mat frame) {
    Mat frame_HSV, gamma_frame;
    vector<Mat> hsv_channels(3);

    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
    split(frame_HSV, hsv_channels);

    // Calcutate gamma pixel table before transformation
    unsigned char pix[256];
    float gamma = 2.5;
    for (int i = 0; i < 256; i++) {
        pix[i] = saturate_cast<uchar>(pow((float)(i) / 255.0, gamma) * 255.0f);
    }

    // Perform Gamma transformation to I mat
    for (int j = 0; j < hsv_channels[2].rows; j++) {
        for (int i = 0; i < hsv_channels[2].cols; i++) {
            hsv_channels[2].at<uchar>(j, i) = pix[hsv_channels[2].at<uchar>(j, i)];
        }
    }

    merge(hsv_channels, gamma_frame);
    cvtColor(gamma_frame, gamma_frame, COLOR_HSV2BGR);

    imshow("video", gamma_frame);
}

void applyHistEqual(Mat frame) {
    Mat frame_HSV, hist_frame;
    vector<Mat> hsv_channels(3);

    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
    split(frame_HSV, hsv_channels);

    // Equalizing the histogram of I mat
    equalizeHist(hsv_channels[2], hsv_channels[2]);

    merge(hsv_channels, hist_frame);
    cvtColor(hist_frame, hist_frame, COLOR_HSV2BGR);

    imshow("video", hist_frame);
}

void applyColorSlicing(Mat frame) {
    Mat frame_HSV, slicing_frame;
    vector<Mat> hsv_channels(3);

    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
    split(frame_HSV, hsv_channels);

    int rows = frame.rows;
    int cols = frame.cols;
    uchar* h;
    uchar* s;

    // Masking out except Orange color
    for (int j = 0; j < rows; j++) {
        h = hsv_channels[0].ptr<uchar>(j);
        s = hsv_channels[1].ptr<uchar>(j);
        for (int i = 0; i < cols; i++) {
            if (h[i] > 9 && h[i] < 23) continue;
            else s[i] = 0;
        }
    }

    merge(hsv_channels, slicing_frame);
    cvtColor(slicing_frame, slicing_frame, COLOR_HSV2BGR);

    imshow("video", slicing_frame);
}

void applyColorConversion(Mat frame) {
    Mat frame_HSV, conversion_frame;
    vector<Mat> hsv_channels(3);

    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
    split(frame_HSV, hsv_channels);

    int rows = frame.rows;
    int cols = frame.cols;
    uchar* h;
    uchar* s;

    // Changing all color by changing Hue values
    for (int j = 0; j < rows; j++) {
        h = hsv_channels[0].ptr<uchar>(j);
        s = hsv_channels[1].ptr<uchar>(j);
        for (int i = 0; i < cols; i++) {
            if (h[i] > 129) h[i] -= 129;
            else h[i] += 50;
        }
    }

    merge(hsv_channels, conversion_frame);
    cvtColor(conversion_frame, conversion_frame, COLOR_HSV2BGR);

    imshow("video", conversion_frame);
}

void applyAverageFiltering(Mat frame) {
    Mat frame_HSV, filtered_frame;
    vector<Mat> hsv_channels(3);

    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
    split(frame_HSV, hsv_channels);

    blur(hsv_channels[2], hsv_channels[2], Size(9, 9)); // Average Filtering of I mat

    merge(hsv_channels, filtered_frame);
    cvtColor(filtered_frame, filtered_frame, COLOR_HSV2BGR);

    imshow("video", filtered_frame);
}

void applySharpening(Mat frame) {
    Mat frame_HSV, blurred, sharpened_frame;
    vector<Mat> hsv_channels(3);

    cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
    split(frame_HSV, hsv_channels);

    blur(hsv_channels[2], blurred, Size(9, 9));

    hsv_channels[2] = hsv_channels[2] + (hsv_channels[2] - blurred);    // Sharpening Filtering of I mat

    merge(hsv_channels, sharpened_frame);
    cvtColor(sharpened_frame, sharpened_frame, COLOR_HSV2BGR);

    imshow("video", sharpened_frame);
}

void applyWhiteBalancing(Mat frame) {
    Mat bgr_channels[3];
    Mat balanced_frame;
    split(frame, bgr_channels);

    double avg;
    int sum, temp, i, j, c;

    for (c = 0; c < frame.channels(); c++) {    // each channels
        sum = 0;
        avg = 0.0f;
        
        // Calculate total sum of channel c
        for (i = 0; i < frame.rows; i++) {
            for (j = 0; j < frame.cols; j++) {
                sum += bgr_channels[c].at<uchar>(i, j);
            }
        }

        // Calculate average of c channel's pixel value
        avg = sum / (frame.rows * frame.cols);

        // Calculate white balancing value
        for (i = 0; i < frame.rows; i++) {
            for (j = 0; j < frame.cols; j++) {
                temp = (128 / avg) * bgr_channels[c].at<uchar>(i, j);
                if (temp > 255) bgr_channels[c].at<uchar>(i, j) = 255;
                else bgr_channels[c].at<uchar>(i, j) = temp;
            }
        }
    }

    merge(bgr_channels, 3, balanced_frame);

    imshow("video", balanced_frame);
}
