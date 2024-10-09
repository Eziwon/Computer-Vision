#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap;

    // open the video
    if (cap.open("Project2_video.mp4") == 0) {
        cout << "no such file!" << endl;
        return -1;
    }

    // get fps and delay per frame of the video
    int fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    Mat background, frame, gray, foregroundMask;
    // set first frame as background
    cap >> background;
    resize(background, background, Size(640, 480));
    cvtColor(background, background, COLOR_BGR2GRAY);

    // set Rect
    Rect rect(250, 240, 100, 240);

    Mat background_roi = background(rect);
    Mat frame_roi;
    int flag = 0;
    while(true) {
        cap >> frame;
        if (frame.empty()) {
            cout << "end of video" << endl;
            break;
        }

        // HoughLine()

        // background substraction <- ROI를 사용해서 앞 차만 확인할 수 있도록 함
        // 내 차는 안움직이고 앞 차만 움직이는 경우
        // resizing the video -> fast 작동

        // Resize the frame to be fast
        resize(frame, frame, Size(640, 480));
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        frame_roi = gray(rect);

        absdiff(background_roi, frame_roi, foregroundMask);
        threshold(foregroundMask, foregroundMask, 100, 1, THRESH_BINARY);

        if ((sum(foregroundMask)[0] > 500) && (flag == 0)){
            cout << "start moving!" << endl;
            flag = 1;
        }

        else if ((sum(foregroundMask)[0] == 0) && (flag == 1)) {
            cout << "stop!" << endl;
            flag = 0;
        }

        if (flag == 1)
            background_roi = frame_roi.clone();
        // 앞이 갑자기 안움직이면? -> update background
        // background 없데이트 이후 움직이면 -> 텍스트 출력
        // 이후 움직임이 없어질 때 까지 그냥 놔둠
        // 또 안움직이면 -> update background


        imshow("video", frame);
        if (waitKey(delay) == 27) break;
    }
    
    return 0;
}