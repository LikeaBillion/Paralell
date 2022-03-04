
#include "Header.h"
#include <mutex>
#include <condition_variable>
#include <queue>


//mutex to avoid thread conflicts
mutex mu;
condition_variable cond;
queue<Images> buffer;


//serial implemntation of grayscaling
void convert_to_grayscale(unsigned char* input, unsigned char* output, int start, int end,
    int channel) {
    //j is need to insure the output is ordered right.
    int j = start;
    int number_of_pixel = end;

    for (int i = start; i < number_of_pixel; i += channel) {
        int blue_value = input[i];
        int green_value = input[i + 1];
        int red_value = input[i + 2];
        //shifts the pixels and then moves them into the output
        output[j++] = (int)(0.114 * blue_value + 0.587 * green_value + 0.299 * red_value);
    }
}

//function to calcualte the euclidian distance between the images
double calculate_distance(unsigned char* testimg, unsigned char* trainimg, int start, int end) {
    double total = 0;
    for (int i = start; i < end; i++) {
        total = +pow((testimg[i] - trainimg[i]), 2);
    }
    double dis = sqrt(total);
    return dis;
    //returns each single distance total
}

//bubble sort written to filter the distances by size
//used in the list version of the code
void bubble_sort_distance(string** distance, int values) {
    for (int i = 0; i < values - 1; i++) {
        for (int j = 0; j < (values - i - 1); j++) {

            if (stoi(distance[0][j]) > stoi(distance[0][j + 1])) {
                //temp stores the values sorts then put them back in the array if needs to
                int temp1 = stoi(distance[0][j]);
                int temp2 = stoi(distance[0][j + 1]);
                string temp3 = distance[1][j];
                string temp4 = distance[1][j + 1
                ];
                distance[0][j] = to_string(temp2);
                distance[0][j + 1] = to_string(temp1);
                distance[1][j] = temp4;
                distance[1][j + 1] = temp3;
            }
        }
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

//list version of the knn calculation- needs to be different to adapt to using arrays for the distances
void calculate_knn_list(string** distance, int kvalue) {
    //variables to hold the counts for all the different types of images to sort
    double daisy_count = 0;
    double dandelion_count = 0;
    double rose_count = 0;
    double sunflower_count = 0;
    double tulip_count = 0;
    int i = 0;

    for (i = 0; i < kvalue; i++) {
        if (distance[1][i] == "daisy") {
            daisy_count++;
        }
        else if (distance[1][i] == "dandelion") {
            dandelion_count++;
        }
        else if (distance[1][i] == "rose") {
            rose_count++;
        }
        else if (distance[1][i] == "sunflower") {
            sunflower_count++;
        }
        else if (distance[1][i] == "tulip") {
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

//function to read images parallely
void read_images(vector<string>& filenames, vector<Images>& train_image_data, int start, int end, int depth) {
    Images train_img;

    if (depth > 3) {
        Mat train;
        string label;
        for (int j = start; j < end; j++) {
            train = imread(filenames[j]);
            label = calculate_label(filenames[j]);
            unique_lock<mutex> lock(mu);
            train_img.train_img = train;
            train_img.label = label;
            Images train_b(train_img);
            buffer.push(train_b);
            cond.notify_all();

        }

    }
    else {
        auto mid = ((start + end) / 2);
        auto left = async(launch::async, read_images, ref(filenames), ref(train_image_data), start, mid, depth + 1);
        read_images(ref(filenames), ref(train_image_data), mid, end, depth + 1);

    }

}

//function for reading the images in a the list implementation- is called for each subfolder
void read_images_list(vector<string>& filenames, list<Mat>& list_train_image_data, list<string>& list_labels) {
    Images train_img;
    for (int j = 0; j < filenames.size(); j++) {
        list_train_image_data.push_back(imread(filenames[j]));
        list_labels.push_back(calculate_label(filenames[j]));
    }
}

//function that pulls together the distance and greyscaling together in one function- and runs through for each of the training images
void distance_handler(vector<Images>& test_image_data, vector<Images>& train_image_data, vector<Images>& distance_data, int test_number, unsigned char* test_output, int start, int end, int depth) {
    //new image vector for distances, this is used so the vector can be cleared after interations to insure that 1 images distances don't bleed into other images data
    Images distance_img;
    //interates through the test data and then the train data for that- calculates distance then the knn before clearing the vector

    if (depth > 3) {
        for (int j = start; j < end; j++) {
            double distance = 0;
            //storing the current training image as variable for easy access
            auto train_img = train_image_data[j].train_img;

            //allocating memory for the train_input and train_outputs
            unsigned char* train_input = (unsigned char*)train_img.data;
            unsigned char* train_output = new unsigned char[train_img.size().width * train_img.size().height];
            const int total_number_of_pixels = train_img.rows * train_img.cols * train_img.channels();
            
            //calculating greyscale
            convert_to_grayscale(train_input, train_output, 0, total_number_of_pixels, train_img.channels());


            //calculating the distance, then storing with: test_name and labels
            distance = calculate_distance(test_output, train_output, 0, (total_number_of_pixels / 3));
            //scoped_lock used to ensure that as soon as a thread leaves scope the resource is available
            scoped_lock<mutex> lock(mu);
            distance_img.distance = distance;
            distance_img.label = train_image_data[j].label;
            distance_img.test_name = test_image_data[test_number].test_name;
            //pushing to image vector of distance
            distance_data.push_back(distance_img);

            //deleting train_output to avoid memory leaks
            delete[] train_output;
        }
    }
    else {
        //using mid for divide and conquor
        auto mid = ((start + end) / 2);
        auto left = async(launch::async, distance_handler, ref(test_image_data), ref(train_image_data), ref(distance_data), test_number, test_output, start, mid, depth + 1);
        distance_handler(ref(test_image_data), ref(train_image_data), ref(distance_data), test_number, test_output, mid, end, depth + 1);
    }
}

//function that pulls together the distance and greyscaling together in one function for the list implementation- and runs through for each of the training images
void distance_handler_list(list<Mat>& list_train_image_data, list<string> list_labels, string** distance, int test_number, unsigned char* test_output, int start, int end, int depth) {
    if (depth > 3) {
        for (int j = start; j < end; j++) {
            //pulling current image from the list
            auto lt = list_train_image_data.begin();
            std::advance(lt, j);
            //storing the current training image as variable for easy access

            Mat train_img = *lt;

            string total = "";

            //allocating memory for the train_input and train_outputs
            unsigned char* train_input = (unsigned char*)train_img.data;
            unsigned char* train_output = new unsigned char[train_img.size().width * train_img.size().height];
            const int total_number_of_pixels = train_img.rows * train_img.cols * train_img.channels();

            //calcuating greyscale
            convert_to_grayscale(train_input, train_output, 0, total_number_of_pixels, train_img.channels());
            //pulling from list labels
            auto ll = list_labels.begin();
            std::advance(ll, j);

            total = to_string(calculate_distance(test_output, train_output, 0, (total_number_of_pixels / 3)));

            //added scoped_lock to ensure resource is available as soon as possible
            scoped_lock<mutex> lock(mu);
            //adding to the calculate distance array- for distance
            distance[0][j] = total;
            //adding to the calculate distance array- for labels
            distance[1][j] = *ll;

            //deleting to avoid memory leaks
            delete[] train_output;
        }
    }
    else {
        //mid used for divide and conquor also
        auto mid = ((start + end) / 2);
        auto left = async(launch::async, distance_handler_list, ref(list_train_image_data), ref(list_labels), distance, test_number, test_output, start, mid, depth + 1);
        distance_handler_list(ref(list_train_image_data), ref(list_labels), distance, test_number, test_output, mid, end, depth + 1);
    }
}

void parallel_main(string dir, string k_value){

    //string to hold the test_img entered using the cin
    string test_img_path;

    //image vectors for testing, training and distance data
    vector<Images> test_image_data;
    vector<Images> train_image_data;
    vector<Images> distance_data;
    list<Mat> list_train_image_data;
    list<string> list_labels;

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
    Images train_img;
    find_subfolders(dir, ref(sub_folders));
    //iterates through all the subfolders adding to the image vector with all the training data and their labels
    for (int i = 0; i < sub_folders.size(); i++) {
        glob(sub_folders[i], filenames);
        read_images(ref(filenames), (train_image_data), 0, filenames.size(), 0);
        //read_images_list(ref(filenames), ref(list_train_image_data), ref(list_labels));
    }

    while (!buffer.empty()) {
        auto bf = buffer.front();
        train_img.train_img = bf.train_img;
        train_img.label = bf.label;
        train_image_data.push_back(train_img);
        buffer.pop();
    }

    //interates through all the test_imgs add puts them into the image vector. Along with the test_name to be used later
    Images test_img;
    glob(test_img_path, test_img_filenames);
    for (int i = 0; i < test_img_filenames.size(); i++) {
        test_img.test_img = (imread(test_img_filenames[i]));
        test_img.test_name = calculate_filename(test_img_filenames[i]);
        test_image_data.push_back(test_img);
    }

    for (int i = 0; i < test_image_data.size(); i++) {
        //storing the current testing image as variable for easy access

        /*string** distance = new string * [2];
        for (int i = 0; i < 2; ++i) {
            distance[i] = new string[list_train_image_data.size()];
        }*/

        auto test_image = test_image_data[i].test_img;

        //allocating memory for the test_input and train_outputs
        unsigned char* test_input = (unsigned char*)test_image.data;
        unsigned char* test_output = new unsigned char[test_image.size().width * test_image.size().height];
        const int total_number_of_pixels = test_image.rows * test_image.cols * test_image.channels();

        //converting test images to greyscale
        convert_to_grayscale(test_input, test_output, 0, total_number_of_pixels, test_image.channels());

        distance_handler(ref(test_image_data), ref(train_image_data), ref(distance_data), i, test_output, 0, train_image_data.size(), 0);
        //distance_handler_list(ref(list_train_image_data), ref(list_labels), distance, i, test_output,0,list_train_image_data.size(),0);


        //function to simply sort the values in the image vector structure
        sort(distance_data.begin(), distance_data.end(), [](Images a, Images b) {return a.distance < b.distance; });
        //bubble_sort_distance(distance,list_train_image_data.size());
        

        //call to calculate knn using the sorted values
        calculate_knn(ref(distance_data), stoi(k_value));
        //calculate_knn_list(distance, stoi(k_value));

        //cleared vector to keep processing time down and to remove potenial bias. Deleting distace (in the array implementation) and testoutput to avoid memory leaks 
        distance_data.clear();
        //delete[] distance;
        delete[] test_output;
    }

    //ending timing
    auto end = steady_clock::now();
    //calculating duration
    double duration_p = duration_cast<milliseconds> (end - start).count();
    cout << "----------------------------------------------------------" << endl;
    //outputing parallel time
    cout << "Paralell time: " << duration_p / 1000 << endl;

}

int main(int argc, char** argv)
{
    std::cout << "starting opencv...." << std::endl;
    
    //storing the passed arguments for directory and k-values
    string dir(argv[1]);
    string k_value(argv[2]);
    //try catch for validation of arguments
    try {
        parallel_main(dir, k_value);
    }
    catch (const invalid_argument& e) {
        cerr << "Invalid arguments given: " << e.what() << endl;
    }

    return 0;
}