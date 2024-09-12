#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Read an image “moon.png” as a grayscale image
    Mat moon = imread("moon.png", 0);
    if (moon.empty()) {
        cout << "cannot found moon.png" << endl;
        return -1;
    }

    Mat moon_blurred, moon_sharpened = moon.clone();

    // Apply Gaussian blur befor sharpening
    GaussianBlur(moon, moon_blurred, Size(9, 9), 1.5);

    // Apply unsharp masking
    moon_sharpened = moon + 1.5 * (moon - moon_blurred);

    // left half: moon_blurred,     right half: sharpened_moon
    Mat moon_filtered = moon_blurred.clone();
    Rect rect_right(moon.cols / 2, 0, moon.cols / 2, moon.rows);
    moon_sharpened(rect_right).copyTo(moon_filtered(rect_right));

    // Read an image "saltnpepper.png” as a grayscale image
    Mat saltnpepper = imread("saltnpepper.png", 0);
    if (saltnpepper.empty()) {
        cout << "cannot found saltnpepper.png" << endl;
        return -1;
    }

    Mat saltnpepper_filtered = saltnpepper.clone();

    // Apply median blur, aperture size as 9
    Mat saltnpepper_blurred;
    medianBlur(saltnpepper, saltnpepper_blurred, 9);

    // left half: saltnpepper_blurred,     right half: saltnpepper
    Rect rect_left(0, 0, saltnpepper.cols / 2, saltnpepper.rows);
    saltnpepper_blurred(rect_left).copyTo(saltnpepper_filtered(rect_left));

    // Display results
    imshow("moon", moon);
    imshow("moon_filtered", moon_filtered);
    imshow("saltnpepper", saltnpepper);
    imshow("saltnpepper_filtered", saltnpepper_filtered);

    waitKey(0);

    return 0;
}