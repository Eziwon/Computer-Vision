#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat gray_image;
    Mat rotate_image, trans_image;

    gray_image = imread("lena.png", 0);

    // Deep copy for rotate the original gray image
    rotate_image = gray_image.clone();

    // Rotate image as rotate_image
    for (int j = 0; j < gray_image.rows; j++) {
        for (int i = 0; i < gray_image.cols; i++) {
            rotate_image.at<uchar>(gray_image.cols - 1 - i, j) = gray_image.at<uchar>(j, i);
        }
    }

    // Deep copy for transformation the rotated image
    trans_image = rotate_image.clone();

    // Calcutate gamma pixel table before transformation
    unsigned char pix[256];
    float gamma = 10;
    for (int i = 0; i < 255; i++) {
        pix[i] = saturate_cast<uchar>(pow((float)(i / 255.0), gamma) * 255.0f);
    }

    // Perform negative or gamma transformation
    unsigned char current_pixel_value;
    for (int j = 0; j < rotate_image.rows; j++) {
        for (int i = 0; i < rotate_image.cols; i++) {
            current_pixel_value = rotate_image.at<uchar>(j, i);

            // if pixel is smaller than 127 -> negative transformation
            if (current_pixel_value < 128) {
                trans_image.at<uchar>(j, i) = 255 - current_pixel_value;
            } else {  // else -> gamma transformation
                trans_image.at<uchar>(j, i) = pix[rotate_image.at<uchar>(j, i)];
            }
        }
    }

    // Show result
    imshow("gray image", gray_image);
    imshow("result", trans_image);

    waitKey(0);

    return 0;
}