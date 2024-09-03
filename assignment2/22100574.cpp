#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat image;
    Mat rotate_image;

    image = imread("lena.png", 0);
    rotate_image = image.clone();

    // Rotate image as rotate_image
    for (int j = 0; j < image.rows; j++) {
        for (int i = 0; i < image.cols; i++) {
            rotate_image.at<uchar>(j, i) = image.at<uchar>(i, j);
        }
    }

    imshow("image", image);
    imshow("rotate image", rotate_image);

    waitKey(0);

    return 0;
}