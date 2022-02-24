#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <future>
#include <filesystem>
#include <math.h>
#include <filesystem>
#include <algorithm>
#include <execution>

using namespace cv;
using namespace std;
using namespace chrono;


//structure for storing images
struct Images {
    string label;
    Mat train_img;
    double distance;

    ~Images() {};

    friend ostream& operator<<(ostream& out, const Images& r);

};

//simple template for vectors
template <typename T>
void t_print(vector<T>& v) {
    if (v.empty()) return;
    for (T& i : v) cout << i << endl;
}

//overloaded the<< operator for type image
//also added a counter int that just outputs the count of the element printing out- useful for debugging
int counter = 0;
ostream& operator<<(ostream& out, const Images& r) {
    counter++;
    out << "{ counter: " << counter << " label: " << r.label << " distence: " << r.distance << " }" << endl;
    return out;
}

//function to sort the distences when in the Image format
bool compare_distance(Images a, Images b) {
    if (a.distance < b.distance) {
        return true;
    } return false;
}

//serial implemntation of grayscaling
void convert_to_grayscale_serial(unsigned char* input, unsigned char* output, int start, int end,
    int channel) {
    int j = start;
    int number_of_pixel = end;

    for (int i = start; i < number_of_pixel; i += channel) {
        int blue_value = input[i];
        int green_value = input[i + 1];
        int red_value = input[i + 2];

        output[j++] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
    }
}
//function to calculate greyscale for the train_images then put them into a vector 
unsigned char* calculate_grey_scale_train(vector<Images>& images, int value) {
    unsigned char* input = (unsigned char*)images[value].train_img.data;
    unsigned char* output = new unsigned char[images[value].train_img.size().width * images[value].train_img.size().height];
    const int total_number_of_pixels = images[value].train_img.rows * images[value].train_img.cols * images[value].train_img.channels();
    convert_to_grayscale_serial(input, output, 0, total_number_of_pixels, 3);
    return output;
}

//function to calculate greyscale for the test_images then put them into a vector 
unsigned char* calculate_grey_scale_test(vector<Mat>& images, int value) {
    unsigned char* input = (unsigned char*)images[value].data;
    unsigned char* output = new unsigned char[images[value].size().width * images[value].size().height];
    const int total_number_of_pixels = images[value].rows * images[value].cols * images[value].channels();
    convert_to_grayscale_serial(input, output, 0, total_number_of_pixels, 3);
    return output;
}

//function to calcualte the euclidian distance between the images
void calculate_distance(unsigned char* testimg, unsigned char* trainimg, int start, int end, vector<Images>& distance, string label) {
    Images img;
    double sum = 0;
    for (int i = start; i < end; i++) {
        sum = +pow((testimg[i] - trainimg[i]), 2);
    }
    double dis = sqrt(sum);
    img.distance = dis;
    img.label = label;
    distance.push_back(img);
}

//function to calculate the knn of the images- simply counts them and then prints the most common
void calculate_knn(vector<Images>& distance, int kvalue) {
    double daisy_count = 0;
    double dandelion_count = 0;
    double rose_count = 0;
    double sunflower_count = 0;
    double tulip_count = 0;

    for (int i = 0; i < kvalue; i++) {
        if (distance[i].label == "daisy") {
            daisy_count++;
        }
        else if (distance[i].label == "dandelion") {
            dandelion_count++;
        }
        else if (distance[i].label == "rose") {
            rose_count++;
        }
        else if (distance[i].label == "sunflower") {
            sunflower_count++;
        }
        else if (distance[i].label == "tulip") {
            tulip_count++;
        }

    }
    cout << "----------------------------------------------------------" << endl;
    cout << "Daisy with: " << daisy_count << " counts" << endl;
    cout << "Dandelion with: " << dandelion_count << " counts" << endl;
    cout << "Rose with: " << rose_count << " counts" << endl;
    cout << "Sunflower with: " << sunflower_count << " counts" << endl;
    cout << "Tulip with: " << tulip_count << " counts" << endl;
    if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == daisy_count) {
        cout << "Daisy with: " << 100 * (daisy_count / kvalue) << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == dandelion_count) {
        cout << "Dandelion with: " << 100 * (dandelion_count / kvalue) << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == rose_count) {
        cout << "Rose with: " << 100 * (rose_count / kvalue) << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == sunflower_count) {
        cout << "Sunflower with: " << 100 * (sunflower_count / kvalue) << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == tulip_count) {
        cout << "Tulip with: " << 100 * (tulip_count / kvalue) << endl;
    }

}
//function to calculate label from the filename provided, just chops off anything after the last // and then before the new last //
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
//function to work out the subfolders from the path provideded by the command line 
void find_subfolders(string dir, vector<string>& subfolders) {
    int label_counter = 0;
    for (const auto& entry : filesystem::directory_iterator(dir)) {
        string path_string{ entry.path().u8string() };
        subfolders.push_back(path_string);
    }

}

int main(int argc, char** argv)
{
    std::cout << "starting the opencv basic.cpp...." << std::endl;
    //validating arguments
    if (argc != 2) {
        cout << "Error in arguments..\n";
        exit(-1);
    }

    //intiallizing vectors for image, filenames(train image), subfolders and test_img filenames and astring for the path of the test images
    string dir(argv[1]);
    Images img;
    vector<Images> image;
    vector<String> filenames;
    vector<String> sub_folders;
    vector<String> test_img_filenames;
    vector<Mat> test_imgs;
    string test_img_path;

    auto start = steady_clock::now();

    cout << "Input test directory: " << endl;
    cin >> test_img_path;

    //function calls to find all the subfolders
    find_subfolders(dir, ref(sub_folders));
    int read_count = 0;
    //iterates through all the subfolders adding to the image vector with all the training data and their labels
    for (int i = 0; i < sub_folders.size(); i++) {
        glob(sub_folders[i], filenames);
        for (int j = 0; j < filenames.size(); j++) {
            img.train_img = (imread(filenames[j]));
            img.label = calculate_label(filenames[j]);
            image.push_back(img);
            read_count++;
        }
    }
    int read_count_test = 0;
    //interates through all the test_imgs add puts them into the image vector
    glob(test_img_path, test_img_filenames);
    for (size_t i = 0; i < test_img_filenames.size(); i++) {
        test_imgs.push_back(imread(test_img_filenames[i]));
        read_count_test++;
    }

    int total_pixels = image[0].train_img.cols * image[0].train_img.rows;
    cout << total_pixels << endl;

    //new image vector for distances, this is used so the vector can be cleared after interations to insure that 1 images distances don't bleed into other images data
    vector<Images>distance;
    //interates through the test data and then the train data for that- calculates distance then the knn before clearing the vector
    for (int i = 0; i < (read_count_test); i++) {
        unsigned char* test_grey = calculate_grey_scale_test(ref(test_imgs), i);
        for (int j = 0; j < read_count; j++) {
            unsigned char* train_grey = calculate_grey_scale_train(ref(image), j);
            calculate_distance(test_grey, train_grey, 0, total_pixels, ref(distance), image[j].label);
            delete[] train_grey;
        }
        sort(distance.begin(), distance.end(), [](Images a, Images b) {return a.distance < b.distance; });
        t_print(distance);
        calculate_knn(ref(distance), 61);
        delete[] test_grey;
        distance.clear();
    }

    auto end = steady_clock::now();
    auto duration_s = duration_cast<microseconds> (end - start).count();
    cout << "Parallel time: " << duration_s << endl;

    return 0;
}
