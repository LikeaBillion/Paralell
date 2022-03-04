#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <future>
#include <filesystem>
#include <math.h>
#include <filesystem>



using namespace cv;
using namespace std;
using namespace chrono;

//structure for storing images- test, training and the distences
struct Images {
    string label;
    string test_name;
    Mat train_img;
    Mat test_img;
    Mat grey_img;
    double distance;

    ~Images() {};

    friend ostream& operator<<(ostream& out, const Images& r);

};

string calculate_label(string& dir) {
    string directory;
    size_t last_slash_idx = dir.rfind('\\');
    size_t end = dir.size();
    if (std::string::npos != last_slash_idx)
    {
        directory = dir.substr(0, last_slash_idx);
        size_t second_last_slash = directory.rfind('\\');
        directory = directory.substr(second_last_slash + 1, end);
    }
    return directory;
}
//function to extract the filename from the path fo the test images
string calculate_filename(string& dir) {
    string name;
    size_t last_slash_idx = dir.rfind('\\');
    size_t end = dir.size();
    if (std::string::npos != last_slash_idx)
    {
        name = dir.substr(last_slash_idx + 1, end);

    }
    return name;
}

//function to work out the subfolders from the path provideded by the command line 
void find_subfolders(string dir, vector<string>& subfolders) {
    int label_counter = 0;
    for (const auto& entry : filesystem::directory_iterator(dir)) {
        string path_string{ entry.path().u8string() };
        subfolders.push_back(path_string);
    }

}
