#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <fstream>
#include "../neural_network/Network.hpp"
#include "../neural_network/FCLayer.hpp"
#include "../neural_network/ActivationLayer.hpp"
#include "../neural_network/Activations.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <thread>
#include <vector>
#include <pthread.h>
#include <utility>
#include <iostream>
#include <string>
#include <fstream>
#include "../sockets/worker.cpp"
#include <sstream>

// ################# NEURAL NETWORK ATTRIBUTES ############################
// network attributes
int n_samples, n_features, n_classes, epochs;

// my x_train and y_train
vector<vector<double>> my_x_train;
vector<vector<double>> my_y_train;
// read in the entire data
vector<vector<double>> x_train;
vector<vector<double>> y_train;

Network network;
bool phase_init = false;
bool work_index_init = false;
bool init_complete = false;
string network_string = "";
string work_index_string = "";
vector<int> data_points;
// ########################################################################
// ################ SOCKET PROGRAMMING ATTRIBUTES ########################
bool start_signal = false;

void construct_network(vector<vector<vector<double>>> weights, vector<vector<double>> biases, vector<string> activations) {
    // Construct the network
    cout << "Constructing network..." << endl;
    // null out the network
    network = Network();

    // alternate looping between weights/biases and activations
    int count = 0;
    for (int i = 0; i < weights.size(); i++) {
        vector<vector<double>> layer_weights = weights[i];
        vector<double> layer_biases = biases[i];
        string activation = activations[i];

        // create a new FCLayer
        FCLayer* fc_layer = new FCLayer(layer_weights, layer_biases);
        network.add(fc_layer);

        // create a new ActivationLayer
        ActivationLayer* activation_layer;
        if (activation == "tanh_1") {
            activation_layer = new ActivationLayer(tanh_1, tanh_prime, "tanh");
        } else if (activation == "relu") {
            activation_layer = new ActivationLayer(relu, relu_prime, "relu");
        } else if (activation == "sigmoid") {
            activation_layer = new ActivationLayer(sigmoid, sigmoid_prime, "sigmoid");
        } else if (activation == "leaky_relu") {
            activation_layer = new ActivationLayer(leaky_relu, leaky_relu_prime, "leaky_relu");
        } else {
            cout << "Invalid activation function: " << activation << endl;
            return;
        }
        network.add(activation_layer);
    }


    network.use(mse, mse_prime);
    cout << "Network constructed." << endl;
}

void handle_init_message(string message) {
    istringstream stream(message);
    vector<vector<vector<double>>> weights;
    vector<vector<double>> biases;
    vector<string> activations;

    string line, word;
    bool initSection = false;
    bool readingBiases = false; 

    while (getline(stream, line)) {
        if (line == "INITSTART") {
            initSection = true;
            continue;
        } else if (line == "INITEND") {
            break;
        }

        if (!initSection) {
            continue;
        }

        if (line.find("weights:") != string::npos) {
            readingBiases = false;
            weights.push_back(vector<vector<double>>()); // Start a new layer of weights
            continue;
        } else if (line.find("biases:") != string::npos) {
            readingBiases = true;
            continue;
        }

        if (line.find("tanh") != string::npos || line.find("relu") != string::npos || 
            line.find("sigmoid") != string::npos || line.find("mse") != string::npos) {
            activations.push_back(line);
            continue;
        }

        istringstream lineStream(line);
        vector<double> dataRow;

        while (lineStream >> word) {
            if (isdigit(word[0]) || word[0] == '-' || word[0] == '.') {
                dataRow.push_back(stod(word));
            }
        }

        if (!dataRow.empty()) {
            if (readingBiases) {
                biases.push_back(dataRow);
            } else if (!weights.empty()) {
                weights.back().push_back(dataRow); // Add the row to the last layer of weights
            }
        }
        
    }

    construct_network(weights, biases, activations);
}


void handle_workers_index_message(string message)
{
    vector<int> numbers;
    string startDelimiter = "WORKINDEXSTART";
    string endDelimiter = "WORKINDEXEND";

    auto startPos = message.find(startDelimiter);
    auto endPos = message.find(endDelimiter);

    if (startPos != std::string::npos && endPos != std::string::npos)
    {
        startPos += startDelimiter.length();
        string numSequence = message.substr(startPos, endPos - startPos);

        std::stringstream ss(numSequence);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            try
            {
                numbers.push_back(std::stoi(item));
            }
            catch (const std::invalid_argument &e)
            {
                // Handle the case where stoi conversion fails
                std::cerr << "Invalid number found: " << item << std::endl;
            }
        }
    }

    data_points = numbers;
}

void get_my_data_points()
{
    // for each data point in data_points
    for (int i = 0; i < data_points.size(); i++)
    {
        // add the x_train and y_train data points to worker_x_train and worker_y_train
        my_x_train.push_back(x_train[data_points[i]]);
        my_y_train.push_back(y_train[data_points[i]]);
    }
}

void handle_message(char *message)
{
    // if its in INIT message, the first four characters are INIT
    string msg(message);
    // if the last eight characters are INITEND, then we are done with INIT
    if (msg.find("INITEND") != string::npos)
    {
        // cout << "INITEND message received. Processing init messages;" << endl;
        phase_init = false;
        network_string += message;
        handle_init_message(network_string);
    }
    else if (strncmp(message, "INITSTART", 8) == 0)
    {
        // cout << "INITSTART message received. Processing init messages;" << endl;
        phase_init = true;
        network_string += message;
    }
    else if (phase_init)
    {
        // cout << "INIT message received. Processing init messages;" << endl;
        network_string += message;
    }

    // do everything above but for WORKINDEX
    if (msg.find("WORKINDEXEND") != string::npos)
    {
        // cout << "WORKINDEXEND message received. Processing work index messages;" << endl;
        work_index_init = false;
        work_index_string += message;
        handle_workers_index_message(work_index_string);
        init_complete = true;
    }
    else if (strncmp(message, "WORKINDEXSTART", 14) == 0)
    {
        // cout << "WORKINDEXSTART message received. Processing work index messages;" << endl;
        work_index_init = true;
        work_index_string += message;
    }
    else if (work_index_init)
    {
        // cout << "WORKINDEX message received. Processing work index messages;" << endl;
        work_index_string += message;
    }
}

void create_reading_thread()
{
    reading_t = thread([&]()
                       {
        while(!end_thread) {
            bzero(buffer,256);
            n = read(sockfd,buffer,255);
            if (n < 0) {
                 perror("perror reading from socket");
            } else {
                 handle_message(buffer);
            }
        } });
}

void readInFile() {
    n_samples = 10;
    n_features = 5;
    n_classes = 2;
    // READ IN THE DATA
    string file_name = "../data/generated_" + to_string(n_samples) + "_" + to_string(n_classes) + "_" + to_string(n_features) + ".txt";
    const char * c = file_name.c_str();
    freopen(c, "r", stdin);
    cout << "Reading in file: " << file_name << endl;
    // read the first line of the file
    string s;
    cin >> s;
    // the next n_samples lines are the x_train
    for (int i = 0; i < n_samples; i++) {
        vector<double> x;
        for (int j = 0; j < n_features; j++) {
            double temp;
            cin >> temp;
            x.push_back(temp);
        }
        // cout << "completed reading x_train[" << i << "]" << endl;
        x_train.push_back(x);
    }

    // Read y_train the next line of the file is 'Y' folllowed by n_samples lines of n_classes
    // the next line is 'Y'
    cin >> s;
    // the next n_samples lines are the y_train
    for (int i = 0; i < n_samples; i++) {
        vector<double> y;
        for (int j = 0; j < n_classes; j++) {
            double temp;
            cin >> temp;
            y.push_back(temp);
        }
        // cout << "completed reading y_train[" << i << "]" << endl;
        y_train.push_back(y);
    }
    // call get_data_points() to populate my_x_train and my_y_train
}

int main(int argc, char *argv[])
{

    end_thread = false;
    port = 8000;
    epochs = 5;

    open_socket();
    string host_name = "hydra.cs.utexas.edu";
    char *host = &host_name[0];
    get_host(host);
    establish_connection();
    create_reading_thread();

    while(!init_complete) {
        sleep(1);
    }

    cout << "Network string created. Initializing network..." << endl;
    cout << "Work index string created. Initializing work index..." << endl;
    cout << "-----------------------------------------------" << endl;
    readInFile();
    cout << "Read in file complete." << endl;
    cout << "-----------------------------------------------" << endl;
    // print out data points
    get_my_data_points();
    cout << "Got my data points." << endl;
    // print out size of my_x_train and my_y_train
    cout << "my_x_train size: " << my_x_train.size() << " original x_train size: " << x_train.size() << endl;
    cout << "my_y_train size: " << my_y_train.size() << " original y_train size: " << y_train.size() << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "Network Initialization complete. Starting training..." << endl;

    for (int i = 0; i < epochs; i++)
    {
        cout << "-----------------------------------------------" << endl;
        cout << "Waiting for master to send start signal..." << endl;

        cout << "Starting Epoch " << i + 1 << "/" << epochs << endl;
        network.fitOneEpoch(my_x_train, my_y_train, 0.1);
        cout << "Finished Epoch " << i + 1 << "/" << epochs << endl;
        string net_at_epoch_end = network.network_string();
        // print cstr2 to a file for debugging
        ofstream myfile;
        myfile.open ("worker_output.txt");
        myfile << net_at_epoch_end;
        myfile.close();
        // DATAP_WORKER_START and DATAP_WORKER_END to the string
        string start = "DATAP_WORKER_START\n";
        string end = "DATAP_WORKER_END\n";
        string combined = start + net_at_epoch_end + end;
        const char *cstr = combined.c_str();
        char* cstr2 = new char[combined.length() + 1];
        strcpy(cstr2, cstr);
        send_message(cstr2);

        cout << "Sent results to master..." << endl;
        cout << "-----------------------------------------------" << endl;
    }

    
    join_thread();

    return 0;
}

// model parallelization per layer
int modelRunSeq(vector<vector<double>> &weight, vector<vector<double>> &biases, vector<vector<double>> &x_train, vector<vector<double>> &y_train)
{

    Network network;
    // TODO: create network with read in weights and biases

    int num_epochs = 5;

    for (int x = 0; x < 50; x++)
    {
        // network.fitOneEpoch(weight, biases, 0.1);
        string weightsToSend = network.getWeights();

        // TODO: send that shit in to the master

        // TODO: read in new net
    }

    // network done!!
    send_results();
    return 0;
}

// data parallelism
int runSequential_data(Network network)
{

    int num_epochs = 50;

    for (int x = 0; x < 50; x++)
    {
        network.fitOneEpoch(my_x_train, my_y_train, 0.1);

        string toSend = network.network_string();

        // TODO: send in new wieghts and biases to master

        // TODO: WAIT & read in new weights and biases net
        // same method to read in init
    }

    // network done!!

    return 0;
}

int runParallelHelper_data(vector<vector<double>> &thread_x_train, vector<vector<double>> &thread_y_train, Network network)
{

    int num_epochs = 50;

    // add layers and shit to network passed in

    for (int x = 0; x < 50; x++)
    {
        network.fitOneEpoch(thread_x_train, thread_y_train, 0.1);

        string netToSend = network.network_string();
        // barrier for threads
        // aggregate all their weights

        // TODO: send that to master

        // TODO: read in new net
    }
    return 0;
}

int runParallel_data(vector<vector<double>> &x_train, vector<vector<double>> &y_train)
{
    // use
    int n_threads = 4;

    // TODO: create network
    Network network;

    network.fitOneEpoch(x_train, y_train, 0.1);

    // partition data
    vector<vector<vector<double>>> x_train_parts(n_threads);
    vector<vector<vector<double>>> y_train_parts(n_threads);

    size_t chunk_size = x_train.size() / n_threads;
    size_t remainder = x_train.size() % n_threads;

    for (int i = 0; i < n_threads; ++i)
    {
        size_t start_idx = i * chunk_size;
        size_t end_idx = (i + 1) * chunk_size + (i == n_threads - 1 ? remainder : 0);

        x_train_parts[i] = vector<vector<double>>(x_train.begin() + start_idx, x_train.begin() + end_idx);
        y_train_parts[i] = vector<vector<double>>(y_train.begin() + start_idx, y_train.begin() + end_idx);
    }

    // Create threads and run runParallelHelper
    vector<thread> threads;
    for (int i = 0; i < n_threads; ++i)
    {
        threads.push_back(thread(runParallelHelper_data, ref(x_train_parts[i]), ref(y_train_parts[i]), network));
    }

    // Wait for all threads to finish
    for (auto &th : threads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }

    // net trained!!!
    cout << "Network trained" << endl;

    return 0;
}
