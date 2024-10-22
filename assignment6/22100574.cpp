#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    string filename, file_path;
    string folder_path = "./DBs/";
    vector<string> glob_result;

    cout << "Enter query image name: ";
    getline(cin, filename);

    file_path = folder_path + filename;
    glob(file_path, glob_result);
    if (glob_result.size() == 0) {
        cout << "No file!" << endl;
        return -1;
    }

    Mat query = imread(file_path);
    
    imshow("query", query);
    waitKey(0);

    return 0;
}