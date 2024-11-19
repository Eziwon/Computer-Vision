#include "opencv2/opencv.hpp"
#include <iostream>
// #include <opencv2/highqui.h>

using namespace cv;
using namespace std;

int main () {
    // open vidoe webcam
    VideoCapture cap = VideoCapture(0);
    int successes = 0;

    int numBoards = 49;
    int numCornersHor = 7;
    int numCornersVer = 7;
    int Rect_size = 20;

    int numSquares = (numCornersHor - 1) * (numCornersVer - 1);
    Size board_sz = Size(numCornersHor, numCornersVer);

    // Container
    vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;
    vector<Point2f> corners;
    vector<Point3f> obj;

    for (int i = 0; i < numCornersHor; i++) {
        for (int j = 0; j < numCornersVer; j++) {
            obj.push_back(Point3f(i*Rect_size, j*Rect_size, 0.0f));
        }
    }

    Mat img, gray;
    cap >> img;
    cout << "image size:" << img.size() << endl;

    while (successes < numBoards) {
        cap >> img;
        cvtColor(img, gray, COLOR_BGR2GRAY);
        if(img.empty()) break;
        if(waitKey(1) == 27) break;

        // Finds the positions of internal corners of the chessboard
        bool found = findChessboardCorners(gray, board_sz, corners, CALIB_CB_ADAPTIVE_THRESH);

        if (found == 1) {
            cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                        TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            
            drawChessboardCorners(img, board_sz, corners, found);
            
            image_points.push_back(corners);
            object_points.push_back(obj);
            printf("Snap stored!\n");
            successes++;
        }

        imshow("win1", img);
        imshow("win2", gray);
        waitKey(1000);
    }

    cout << "Complete!" << endl;

    Mat intrinsic = Mat(3, 3, CV_32FC1);
    Mat disCoeffs;
    vector<Mat> rvecs;
    vector<Mat> tvecs;

    intrinsic.ptr<float>(0)[0] = 1;
    intrinsic.ptr<float>(1)[1] = 1;

    calibrateCamera(object_points, image_points, img.size(), intrinsic, disCoeffs, rvecs, tvecs);

    // Mat imageUndistorted;

    // Print result
    cout << "==================Intrinsic Parameter=====================" << "\n";
    for (int i = 0; i < intrinsic.rows; i++){
        for (int j = 0; j < intrinsic.cols; j++) {
            cout << intrinsic.at<double>(i, j)<<"\t\t";
        }
        cout << endl;
    }
    cout << "==========================================================" << "\n";

    waitKey();
    return 0;
}