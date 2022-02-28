#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <future>
#include <filesystem>
#include <math.h>
#include <filesystem>
#include <mutex>
#include <shared_mutex>


using namespace cv;
using namespace std;
using namespace chrono;
mutex mu;
mutex mu2;


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

//simple template for printing vectors
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
    out << "{ counter: " << counter << " label: " << r.label << " file name: " << r.test_name << " distence: " << r.distance << " }" << endl;
    return out;
}

//serial implemntation of grayscaling
void convert_to_grayscale(unsigned char* input, unsigned char* output, int start, int end, int channel, int depth) {
    if (depth > 4) {
        int j = start;
        int number_of_pixel = end;

        for (int i = start; i < number_of_pixel; i += channel) {
            int blue_value = input[i];
            int green_value = input[i + 1];
            int red_value = input[i + 2];

            int out_index = i / channel;

            output[out_index] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
        }
    }

    else {
        auto mid = ((start + end) / 2);
        auto left = async(launch::async, convert_to_grayscale, input, output, start, mid, channel, depth + 1);
        convert_to_grayscale(input, output, mid, end, channel, depth + 1);
    }

}

//function to calcualte the euclidian distance between the images
void calculate_distance(unsigned char* testimg, unsigned char* trainimg, int start, int end, double& total, int depth) {
    Images img;
    if (depth > 4) {

        for (int i = start; i < end; i++) {
            total = +pow((testimg[i] - trainimg[i]), 2);
        }
    }
    else {
        auto mid = ((start + end) / 2);
        auto left = async(launch::async, calculate_distance, testimg, trainimg, start, mid, ref(total), depth + 1);
        calculate_distance(testimg, trainimg, mid, end, ref(total), depth + 1);
    }
}

//function to calculate the knn of the images- simply counts them and then prints the most common
void calculate_knn(vector<Images> distance, int kvalue) {
    //variables to hold the counts for all the different types of images to sort
    double daisy_count = 0;
    double dandelion_count = 0;
    double rose_count = 0;
    double sunflower_count = 0;
    double tulip_count = 0;
    int i;

    for (i = 0; i < kvalue; i++) {
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
    cout << "Filename: " << distance[i].test_name << endl;
    cout << "Daisy with: " << daisy_count << " counts" << endl;
    cout << "Dandelion with: " << dandelion_count << " counts" << endl;
    cout << "Rose with: " << rose_count << " counts" << endl;
    cout << "Sunflower with: " << sunflower_count << " counts" << endl;
    cout << "Tulip with: " << tulip_count << " counts" << endl;
    if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == daisy_count) {
        cout << "Its a daisy with: " << 100 * (daisy_count / kvalue) << " confidence" << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == dandelion_count) {
        cout << "Its a dandelion with: " << 100 * (dandelion_count / kvalue) << " confidence" << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == rose_count) {
        cout << "Its a rose with: " << 100 * (rose_count / kvalue) << " confidence" << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == sunflower_count) {
        cout << "Its a sunflower with: " << 100 * (sunflower_count / kvalue) << " confidence" << endl;
    }
    else if (max({ daisy_count,dandelion_count, rose_count, sunflower_count, tulip_count }) == tulip_count) {
        cout << "Its a tulip with: " << 100 * (tulip_count / kvalue) << " confidence" << endl;
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

void read_images(vector<string>& sub_folders, vector<string>& filenames, vector<Images>& train_image_data, int start, int end, int depth) {
    Images train_img;

    if (depth > 4) {
        scoped_lock<mutex> sl(mu);
        for (int j = start; j < end; j++) {
            train_img.train_img = (imread(filenames[j]));
            train_img.label = calculate_label(filenames[j]);
            train_image_data.push_back(train_img);
        }
        
    }
    else {
        auto mid = ((start + end) / 2);
        auto left = async(launch::async, read_images, ref(sub_folders), ref(filenames), ref(train_image_data), start, mid, depth + 1);
        read_images(ref(sub_folders), ref(filenames), ref(train_image_data), mid, end, depth + 1);

    }

}


int main(int argc, char** argv)
{
    std::cout << "starting opencv...." << std::endl;
    //validating arguments
    if (argc != 3) {
        cout << "Error in arguments..\n";
        exit(-1);
    }

    //storing the passed arguments for directory and k-values
    string dir(argv[1]);
    string k_value(argv[2]);
    //string to hold the test_img entered using the cin
    string test_img_path;

    //image vectors for testing, training and distance data
    vector<Images> test_image_data;
    vector<Images> train_image_data;
    vector<Images> distance_data;

    //vectors to store filename sand test_filenames too
    vector<String> filenames;
    vector<String> test_img_filenames;

    //vector to hold all the subfolders from the main directory
    vector<String> sub_folders;

    //staring timing
    auto start = steady_clock::now();

    //cin the test_img path
    cout << "Input test directory: " << endl;
    cin >> test_img_path;

    //function calls to find all the subfolders
    find_subfolders(dir, ref(sub_folders));
    //iterates through all the subfolders adding to the image vector with all the training data and their labels
    
    
    for (int i = 0; i < sub_folders.size(); i++) {
        
        glob(sub_folders[i], filenames);
        read_images(ref(sub_folders), ref(filenames), ref(train_image_data), 0, filenames.size(), 0);
        
    }


    //interates through all the test_imgs add puts them into the image vector. Along with the test_name to be used later
    Images test_img;
    glob(test_img_path, test_img_filenames);
    for (int i = 0; i < test_img_filenames.size(); i++) {
        test_img.test_img = (imread(test_img_filenames[i]));
        test_img.test_name = calculate_filename(test_img_filenames[i]);
        test_image_data.push_back(test_img);
    }



    //new image vector for distances, this is used so the vector can be cleared after interations to insure that 1 images distances don't bleed into other images data
    Images distance_img;
    //interates through the test data and then the train data for that- calculates distance then the knn before clearing the vector
    for (int i = 0; i < test_image_data.size(); i++) {

        //storing the current testing image as variable for easy access
        auto test_image = test_image_data[i].test_img;

        //allocating memory for the test_input and train_outputs
        unsigned char* test_input = (unsigned char*)test_image.data;
        unsigned char* test_output = new unsigned char[test_image.size().width * test_image.size().height];
        //count for the max number of pixels
        const int total_number_of_pixels = test_image.rows * test_image.cols * test_image.channels();
        const int train_size = train_image_data.size();
        convert_to_grayscale(test_input, test_output, 0, total_number_of_pixels, test_image.channels(),0);


        for (int j = 0; j < train_size; j++) {

            //storing the current training image as variable for easy access
            auto train_img = train_image_data[j].train_img;
            double total = 0;

            //allocating memory for the train_input and train_outputs
            unsigned char* train_input = (unsigned char*)train_img.data;
            unsigned char* train_output = new unsigned char[train_img.size().width * train_img.size().height];

            convert_to_grayscale(train_input, train_output, 0, total_number_of_pixels, train_img.channels(),0);


            //calculating the distance, then storing with: test_name and labels
            calculate_distance(test_output, train_output, 0, (total_number_of_pixels / 3), ref(total), 0);
            distance_img.distance = sqrt(total);
            distance_img.label = train_image_data[j].label;
            distance_img.test_name = test_image_data[i].test_name;
            distance_data.push_back(distance_img);

            delete[] train_output;
        }

        //function to simply sort the values in the image vector strcuture
        sort(distance_data.begin(), distance_data.end(), [](Images a, Images b) {return a.distance < b.distance; });
        //t_print(distance_data);
        //call to calculate knn using the sorted values
        calculate_knn(ref(distance_data), stoi(k_value));
        //cleared vector to keep processing time down and to remove potenial bias
        distance_data.clear();
        delete[] test_output;
    }

    //ending timing
    auto end = steady_clock::now();
    //calculating duration
    double duration_p = duration_cast<milliseconds> (end - start).count();
    cout << "----------------------------------------------------------" << endl;
    //outputing parallel time
    cout << "Parallel time: " << duration_p/1000 << endl;

    return 0;
}
