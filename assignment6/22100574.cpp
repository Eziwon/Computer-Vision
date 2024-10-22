#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    string filename;
    cout << "Enter query image name:" << endl;
    getline(cin, filename);

    string folder_path = "./DBs/";
    string file_path = folder_path + filename;

    vector<string> glob_result;
    glob(file_path, glob_result);

    if (glob_result.size() == 0) {
        cout << "No file!" << endl;
        return -1;
    }

    return 0;
}