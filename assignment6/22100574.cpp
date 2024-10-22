#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main () {
    // Get query image name
    string filename;
    cout << "Enter query image name: ";
    cin >> filename;

    vector<string> query_images_paths;

    // Set query image path
    string query_path = "./query_image/" + filename;

    // Get query image path
    glob(query_path, query_images_paths);

    // If the file does not exit -> error message, end the program
    if (query_images_paths.empty()) {
        cout << "No file!" << endl;
        return -1;
    }

    // Read and resize the qeury image
    Mat query = imread(query_images_paths[0]);
    resize(query, query, Size(640, 480));

    vector<string> db_images_paths;
    vector<Mat> db_images;

    // Get DB images pathes
    glob("./DBs", db_images_paths);

    // Read all of the DB images and resize
    for (int i = 0; i < db_images_paths.size(); i++) {
        Mat image = imread(db_images_paths[i]);
        if (!image.empty()) {
            resize(image, image, Size(640, 480));
            db_images.push_back(image);
        }
    }

    // Print the number of DB images
    cout << "Sample Image Load Size: " << db_images.size() << endl;

    Ptr<ORB> orbF = ORB::create(1000);
    Mat descriptors_query;
    vector<KeyPoint> keypoints_query;
    BFMatcher matcher(NORM_HAMMING);
    
    int k = 2;
    float nndr = 0.6f;

    // These are for best matching image in DBs
    vector<DMatch> bestMatches;
    double best_match_score = 0;
    Mat best_image;
    vector<KeyPoint> best_keypoints;

    // Compute ORB feature of query image
    orbF->detectAndCompute(query, noArray(), keypoints_query, descriptors_query);

    // for each DB images
    for (int i = 0; i < db_images.size(); i++) {
        vector<KeyPoint> keypoints_db;
        Mat descriptors_db;
        vector< vector<DMatch> > matches;
        vector<DMatch> goodMatches;
        
        // Compute ORB feature of a db image
        orbF->detectAndCompute(db_images[i], noArray(), keypoints_db, descriptors_db);

        // Find best and second-best matches by using KNN Matching (k = 2)
        matcher.knnMatch(descriptors_db, descriptors_query, matches, k);

        // Find out the best match is definitely better than the second-best match
        for (int j = 0; j < matches.size(); j++) {
            if (matches.at(j).size() == 2 && matches.at(j).at(0).distance <= nndr * matches.at(j).at(1).distance) {
                goodMatches.push_back(matches[j][0]);
            }
        }
        
        // If an image matches better than the current best images -> update best match image
        if (goodMatches.size() > best_match_score) {
            best_match_score = goodMatches.size();
            best_image = db_images[i];
            bestMatches = goodMatches;
            best_keypoints = keypoints_db;
        }

        // Print the number of matches per image
        cout << "Image number " << (i + 1) << " Matching: " << goodMatches.size() << endl;
    }

    // Draws the found matches of keypoints from best image and query
    Mat best_matching;
    drawMatches(best_image, best_keypoints, query, keypoints_query, bestMatches, best_matching, 
    Scalar::all(-1), Scalar(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // Print whether it matches or not
    if (bestMatches.size() < 4) {
        cout << "Matching failed" << endl;
        return 0;
    } else {
        cout << "Match found!" << endl;
    }

    // Display the result
    imshow("Query", query);
    imshow("best_matching", best_matching);

    waitKey(0);

    return 0;
}