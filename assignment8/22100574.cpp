#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct MouseParams {
    Mat result;
    vector<Point2f> in, out;
    vector<Point> int_out;  // this is for polygon mask
};

static void onMouse(int event, int x, int y, int flags, void *param) {
    MouseParams *mp = (MouseParams*)param;

    if ((mp->out).size() < 4) {
        if (event == EVENT_LBUTTONDOWN) {
            mp->out.push_back(Point2f(x, y));
            mp->int_out.push_back(Point(x, y));
        }
    }
}

int main() {
    VideoCapture cap_contest, cap_timesquare;

    // Open the video
    if (cap_contest.open("contest.mp4") == 0) {
        cout << "no such video!" <<endl;
        return -1;
    }
    if (cap_timesquare.open("Timesquare.mp4") == 0) {
        cout << "no such video!" <<endl;
        return -1;
    }

    // get fps and delay for display
    int fps = cap_timesquare.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    Mat c_frame, t_frame;
    cap_timesquare >> t_frame;
    cap_contest >> c_frame;

    // set 'in' points by using first contest frame
    MouseParams mp;
    mp.in.push_back(Point2f(0, 0));
    mp.in.push_back(Point2f(c_frame.cols, 0));
    mp.in.push_back(Point2f(c_frame.cols, c_frame.rows));
    mp.in.push_back(Point2f(0, c_frame.rows));

    mp.result = t_frame.clone();

    namedWindow("input");
    moveWindow("input", t_frame.cols, 0);

    while(1) {        
        cap_timesquare >> t_frame;
        cap_contest >> c_frame;
        if (t_frame.empty() || c_frame.empty()) {
            cout << " end of video" << endl;
            break;
        }

        // display the frame
        imshow("background", mp.result);
        imshow("input", c_frame);

        if (waitKey(delay) == 27) break;

        // update mp.result after display
        t_frame.copyTo(mp.result);

        // If there are 4 points in 'out' Points vector -> apply perspective transform
        if (mp.out.size() == 4) {
            Mat c_result;

            // apply perspective transform
            Mat homo_mat = getPerspectiveTransform(mp.in, mp.out);
            warpPerspective(c_frame, c_result, homo_mat, t_frame.size());

            // make result frame (using polygone mask of perspective result)
            Mat mask = Mat::zeros(t_frame.size(), CV_8U);
            fillPoly(mask, mp.int_out, Scalar(255));
            c_result.copyTo(mp.result, mask);
        }

        else if (mp.out.size() < 4){
            setMouseCallback("background", onMouse, (void*)&mp);
            
            // draw circle on result frame
            for (int i = 0; i < mp.out.size(); i++) {
                circle(mp.result, mp.out[i], 1, Scalar(0, 0, 255), 5);
            }
        }
    }

    return 0;
}