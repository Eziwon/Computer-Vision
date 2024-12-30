#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    vector<Mat> images;

    // read images
    string folder_path = "./panorama_image/";
    for (int i = 1; i <= 4; i++) {
        string file_path = folder_path + "pano" + to_string(i) + ".JPG";
        Mat img = imread(file_path, IMREAD_COLOR);
        images.push_back(img);
    }

    Ptr<ORB> orbF = ORB::create(1000);
    BFMatcher matcher(NORM_HAMMING);
    Mat base = images[0].clone();

    int global_min_x = 0, global_min_y = 0;
    int global_max_x = base.cols, global_max_y = base.rows;

    for (int i = 1; i < images.size(); i++) {
        Mat descriptors1, descriptors2;
        vector<KeyPoint> keypoints1, keypoints2;
        vector<DMatch> goodMatches;

        Mat next = images[i].clone();

        // detect features and conpute discriptors
        orbF->detectAndCompute(base, noArray(), keypoints1, descriptors1);
        orbF->detectAndCompute(next, noArray(), keypoints2, descriptors2);

        // apply KNN matching
        vector<vector<DMatch>> matches;
        matcher.knnMatch(descriptors1, descriptors2, matches, 2);

        // choose only good matches
        float nndr = 0.6f;
        for (int j = 0; j < matches.size(); j++) {
            if (matches[j].size() == 2 && matches[j][0].distance <= nndr * matches[j][1].distance) {
                goodMatches.push_back(matches[j][0]);
            }
        }

        // extract the coordinates of matching features
        vector<Point2f> points1, points2;
        for (int j = 0; j < goodMatches.size(); j++) {
            DMatch Match = goodMatches[j];
            points1.push_back(keypoints1[Match.queryIdx].pt);
            points2.push_back(keypoints2[Match.trainIdx].pt);
        }

        // compute the homography
        Mat HomoMat = findHomography(points2, points1, RANSAC);

        // compute the coordinates of the corner of the next image
        vector<Point2f> corners(4);
        corners[0] = Point2f(0, 0);
        corners[1] = Point2f(next.cols, 0);
        corners[2] = Point2f(next.cols, next.rows);
        corners[3] = Point2f(0, next.rows);

        vector<Point2f> transformed_corners(4);
        perspectiveTransform(corners, transformed_corners, HomoMat);

        // compute the size of new canvas
        for (int j = 0; j < 4; j++) {
            global_min_x = min(global_min_x, (int)floor(transformed_corners[j].x));
            global_max_x = max(global_max_x, (int)ceil(transformed_corners[j].x));
            global_min_y = min(global_min_y, (int)floor(transformed_corners[j].y));
            global_max_y = max(global_max_y, (int)ceil(transformed_corners[j].y));
        }

        int new_width = global_max_x - global_min_x;
        int new_height = global_max_y - global_min_y;

        // 캔버스 확장
        Mat stitchedImage(new_height, new_width, base.type(), Scalar::all(0));
        Mat translationMat = (Mat_<double>(3, 3) << 1, 0, -global_min_x, 0, 1, -global_min_y, 0, 0, 1);

        // apply warpPerspective
        // INTER_LINEAR: use 4 neighbor pixels for interpolation
        // BORDER_TRANSPARENT: unconverted pixel values ​​remain unchanged
        warpPerspective(base, stitchedImage, translationMat, stitchedImage.size());
        warpPerspective(next, stitchedImage, translationMat * HomoMat, stitchedImage.size(), INTER_LINEAR, BORDER_TRANSPARENT);

        // update the base image
        base = stitchedImage.clone();
    }

    // display the result
    imshow("panorama_img", base);
    waitKey(0);

    return 0;
}