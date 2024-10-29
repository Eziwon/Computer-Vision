#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct CallbackParam {
    Mat frame;
    Point pt1, pt2;
    Rect roi;
    // bool drag;
    bool updated;
};

// void resetProgram(CallbackParam *p, bool *t) {
//     p->updated = false;
//     *t = false;
// }

bool compareBySize(const Rect &a, const Rect &b) {
    return a.area() > b.area(); // Larger area means closer
}

int main () {
    CascadeClassifier face_classifier;
    VideoCapture cap;
    CallbackParam param;
    Mat frame, gray_frame;
    vector<Rect> faces;
    int num_faces;
    int i;
    bool n_key = false, m_key = false, f_key = false, t_key = false;

    // Open the video
    if (cap.open("Faces.mp4") == 0) {
        cout << "no such video!" <<endl;
        return -1;
    }

    // Get fps and delay for display the video
    int fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps;

    // Face detection configuration
    face_classifier.load("haarcascade_frontalface_alt.xml");

    // param.updated = false;
    // bool tracking = false;

    while(1) {
        // Get a next frame
        cap >> frame;
        if(frame.empty()) {
            cout << "end of video!" << endl;
            break;
        }

        // Check the input key
        int key = waitKey(delay);
        if (key == 27) {    // ESC -> end the video
            break;
        } else if(key == 'n') {     // detect the nearest face (제일 앞에 있는 사람 -> scale이 가장 큰 face)
            n_key = true;
            f_key = m_key = false;
        } else if (key == 'f') {    // detect the farthest face (제일 뒤에 있는 사람 -> scale이 가장 작은 face)
            f_key = true;
            n_key = m_key = false;
        } else if (key == 'm') {    // detect the face at the middel range
            m_key = true;
            f_key = n_key = false;
        } else if (key == 'r') {    // reset program
            m_key = f_key = n_key = false;
        } else if (key == 't') {
            // track only the region of face you chose
            // another window를 사용해서 face region만 보여주기 ("tracking" window name)
            // 다시 t를 누르면 destroy window
            // window 지정 전에 t를 누르면 -> "Detect before tracking" message 출력
            t_key = true;
        } 

        if (n_key || m_key || f_key) {
            // Convert the frame as grayscale frame
            cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

            // Detect faces
            face_classifier.detectMultiScale(gray_frame, faces, 1.1, 3, 0, Size(45, 45), Size(80, 80));
            num_faces = faces.size();

            // Sort faces in ascending order
            sort(faces.begin(), faces.end(), compareBySize);
        }

        if (n_key) {
            rectangle(frame, faces[0], Scalar(0, 255, 0), 3);
        } 
        else if (m_key) {
            if (num_faces == 3) rectangle(frame, faces[num_faces-2], Scalar(0, 255, 0), 3);
            else rectangle(frame, faces[num_faces-1], Scalar(0, 255, 0), 3);
        } 
        else if (f_key) {
            rectangle(frame, faces[num_faces-1], Scalar(0, 255, 0), 3);
        }

        imshow("Faces", frame);
    }
    
    return 0;
}