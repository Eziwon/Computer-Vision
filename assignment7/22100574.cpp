#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct CallbackParam {
    Rect roi;
    bool updated;
};

int main () {
    CascadeClassifier face_classifier;
    VideoCapture cap;
    CallbackParam param;
    Mat frame, gray_frame, hsv_frame;
    vector<Rect> faces;
    int num_faces;
    int i;
    bool n_key = false, m_key = false, f_key = false, tracking = false, t_error = false;
    int t_count = 0;

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

    float hrange[] = {0, 180};
    float srange[] = {0, 255};
    float vrange[] = {0, 255};

    const float* ranges[] = {hrange, srange, vrange};
    int channels[] = {0, 1, 2};
    int hist_sizes[] = {16, 16, 16};

    Mat m_model3d, m_backproj;

    Rect rc;

    while(1) {
        // Get a next frame
        cap >> frame;
        if(frame.empty()) {
            cout << "end of video!" << endl;
            break;
        }
        Mat display_frame = frame.clone();

        // Check the input key
        int key = waitKey(delay);
        if (key == 27) {    // ESC -> end the video
            break;
        } else if(key == 'n') {     // detect the nearest face (제일 앞에 있는 사람 -> scale이 가장 큰 face)
            n_key = true;
            f_key = m_key = param.updated = false;
        } else if (key == 'f') {    // detect the farthest face (제일 뒤에 있는 사람 -> scale이 가장 작은 face)
            f_key = true;
            n_key = m_key = param.updated = false;
        } else if (key == 'm') {    // detect the face at the middel range
            m_key = true;
            f_key = n_key = param.updated = false;
        } else if (key == 'r') {    // reset program
            m_key = f_key = n_key = tracking = t_error = param.updated = false;
            destroyWindow("tracking");
        } else if (key == 't') {    // tracking
            if (n_key || m_key || f_key) {  // case 1: user press any key(n, m, f) before
                // toggle the tracking
                tracking = !tracking;
                t_error = false;
                if (!tracking) {    // turn off the tracking window
                    param.updated = false;
                    destroyWindow("tracking");
                }
            }
            else {  // case 2: without previously pressing any keys(n, m, f)
                t_error = true;
            }
        } 

        if (n_key) {
            // Convert the frame as grayscale frame
            cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

            // Detect faces
            face_classifier.detectMultiScale(gray_frame, faces, 1.1, 3, 0, Size(76, 76), Size(84, 84));

            if(faces.size() == 0) {
                rectangle(display_frame, rc, Scalar(0, 255, 0), 3);
            } else if (faces.size() == 1) { 
                rectangle(display_frame, faces[0], Scalar(0, 255, 0), 3);
                // update the recent rc;
                rc = faces[0];
            } else if (faces.size() == 2) {
                if (faces[0].width >= faces[1].width) {
                    rectangle(display_frame, faces[0], Scalar(0, 255, 0), 3);
                    rc = faces[0];
                } else {
                    rectangle(display_frame, faces[1], Scalar(0, 255, 0), 3);
                    rc = faces[1];
                }
            }
        } 
        else if (m_key) {
            // Convert the frame as grayscale frame
            cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

            // Detect faces
            face_classifier.detectMultiScale(gray_frame, faces, 1.1, 3, 0, Size(53, 53), Size(56, 56));

            if(faces.size() > 0) {
                rectangle(display_frame, faces[0], Scalar(0, 255, 0), 3);

                // update the recent rc;
                rc = faces[0];
            }
        } 
        else if (f_key) {
            // Convert the frame as grayscale frame
            cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

            // Detect faces
            face_classifier.detectMultiScale(gray_frame, faces, 1.1, 3, 0, Size(32, 32), Size(39, 39));

            if(faces.size() > 0) {
                rectangle(display_frame, faces[0], Scalar(0, 255, 0), 3);

                // update the recent rc;
                rc = faces[0];
            }
        } 

        if (t_error) {
            t_count++;
            putText(display_frame, format("Detect before tracking"), Point(10, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

            if (t_count > fps * 2) {
                t_error = false;
                t_count = 0;
            }
        }
        else if (tracking) {
            Rect tracking_rc;

            // Convert frame from RGB to HSV
            cvtColor(frame, hsv_frame, COLOR_BGR2HSV);

            if (!(param.updated)) {
                param.roi = rc;

                // Setting the mask for tracking
                Mat mask = Mat::zeros(param.roi.height, param.roi.width, CV_8U);
                ellipse(mask, Point(param.roi.width / 2, param.roi.height / 2), Size(param.roi.width / 2, param.roi.height / 2), 0, 0, 360, 255, FILLED);
                
                Mat roi(hsv_frame, param.roi);

                // Calculate histogram 
                calcHist(&roi, 1, channels, mask, m_model3d, 3, hist_sizes, ranges);

                param.updated = true;
            }

            tracking_rc = param.roi;

            // Apply backprojection
            calcBackProject(&hsv_frame, 1, channels, m_model3d, m_backproj, ranges);

            // Tracking by using meanShift
            meanShift(m_backproj, tracking_rc, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));

            // Setting the tracking window
            Mat tracking_window = Mat::zeros(frame.size(), frame.type());
            tracking_window.setTo(Scalar(255, 0, 0)); 
            frame(tracking_rc).copyTo(tracking_window(tracking_rc));

            // show the tracking window
            imshow("tracking", tracking_window);

            // update the roi for next tracking
            param.roi = tracking_rc;
        }

        imshow("Faces", display_frame);
    }
    
    return 0;
}