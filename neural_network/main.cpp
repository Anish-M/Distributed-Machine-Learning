#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <fstream>
#include "Network.hpp"
#include "FCLayer.hpp"
#include "ActivationLayer.hpp"
#include "Activations.hpp"
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
#include "../master.cpp"




using namespace std;



// ################# NEURAL NETWORK ATTRIBUTES ############################
int n_samples, n_features, n_classes;
vector<vector<double>> x_train;
vector<vector<double>> y_train;
Network network;
// ########################################################################


int initialize_network() {


    // Read x_train the first line of the file is 'X' folllowed by n_samples lines of n_features
    string s;
    
    // the first line is 'X'
    // open the file ../data/generated8000_2_16000.txt with cin
    string file_name = "../data/generated_" + to_string(n_samples) + "_" + to_string(n_features) + "_" + to_string(n_classes) + ".txt";
    const char * c = file_name.c_str();
    freopen(c, "r", stdin);
    // read the first line of the file
    cin >> s;
    cout << s << endl;
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
    cout << s << endl;
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


    network.add(new FCLayer(n_features, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime, "tanh_1"));
    network.add(new FCLayer(5, 8));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(relu, relu_prime, "relu")); // Add activation layer with sigmoid
    network.add(new FCLayer(8, 4));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(sigmoid, sigmoid_prime, "sigmoid")); // Add activation layer with sigmoid
    network.add(new FCLayer(4, 1));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime, "tanh")); // Add activation layer with sigmoid
    network.use(mse, mse_prime);
    

    return 0;
}

string network_string () {
    string network_str = "";
    network_str += network.getWeights();
    network_str += "\n";
    network_str += network.getBiases();
    network_str += "\n";
    network_str += "mse";
    return network_str;
}

int runExample() {
    // Example usage
    Network network;
    network.add(new FCLayer(3, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime, "tanh_1"));

    network.add(new FCLayer(5, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(relu, relu_prime, "relu"));

    // network.add(new FCLayer(4, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    // network.add(new ActivationLayer(tanh, tanh_prime));
    
    network.add(new FCLayer(5, 1));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(sigmoid, sigmoid_prime, "sigmoid")); // Add activation layer with sigmoid
    network.use(mse, mse_prime);     // Use mean squared error loss


    // Train and predict
    vector<vector<double>> x_train = {{0, 0, 3}, {0, 1, 2}, {1, 0, 1}, {1, 1, 0}, {9, 9, 8}, {9, 8, 7}, {8, 9, 6}, {8, 8, 7}};
    vector<vector<double>> y_train = {{0}, {0}, {0}, {0}, {1}, {1}, {1}, {1}};
    network.fit(x_train, y_train, 50, 0.01);
    vector<double> prediction = network.predict({9, 9, 9});
    for (double val : prediction) cout << val << " ";
    cout << endl;

    return 0;
}

pair<vector<vector<double>>, vector<vector<double>>> readDataMaster(int n_samples, int n_features, int n_classes, const char* filename) {
    vector<vector<double>> x_train;
    vector<vector<double>> y_train;

    // Read x_train and y_train from file data/generated8000_2_16000.txt
    // n_samples/n_features/n_classes
    // // freopen("../data/generated8000_2_16000.txt", "r", stdin);

    // Read x_train the first line of the file is 'X' folllowed by n_samples lines of n_features
    string s;
    
    // the first line is 'X'
    // open the file ../data/generated8000_2_16000.txt with cin
    freopen(filename, "r", stdin);
    // read the first line of the file
    cin >> s;
    cout << s << endl;
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
    cout << s << endl;
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

    // print the x_train and y_train size
    cout << "x_train size: " << x_train.size() << endl;
    cout << "y_train size: " << y_train.size() << endl;

    return make_pair(x_train, y_train);
}

vector<vector<int>> splitDataMaster(int numWorkers) {
    // randomly shuffle the indices, but seed it so that it is the same every time
    srand(0);
    vector<int> indices(n_samples);
    iota(indices.begin(), indices.end(), 0);
    random_shuffle(indices.begin(), indices.end());

    // split the indices into numWorkers chunks
    vector<vector<int>> chunks(numWorkers);
    int chunkSize = n_samples / numWorkers;
    int remainder = n_samples % numWorkers;

    int index = 0;
    for (int i = 0; i < numWorkers; i++) {
        // Determine the size of the current chunk
        int currentChunkSize = chunkSize + (i < remainder ? 1 : 0);

        for (int j = 0; j < currentChunkSize; j++) {
            chunks[i].push_back(indices[index++]);
        }
    }

    // print out the chunks per worker
    for (int i = 0; i < numWorkers; i++) {
        cout << "Worker " << i << ": ";
        for (size_t j = 0; j < chunks[i].size(); j++) {
            cout << chunks[i][j] << " ";
        }
        cout << endl;
    }

    return chunks;
}



vector<string> getStringToSendClients(vector<vector<int>> indices, int numWorkers) {
    vector<string> stringsToSend;
    for (int i = 0; i < numWorkers; i++) {
        string s = "";
        for (int j = 0; j < indices[i].size(); j++) {
            s += to_string(indices[i][j]) + ",";
        }
        stringsToSend.push_back(s);
    }
    return stringsToSend;
}



int readInDataWorker(int numWorkers, string indices_to_read) {
    
    // Read x_train and y_train from file
    vector<vector<double>> x_train;
    vector<vector<double>> y_train;

    // Read x_train and y_train from file 
    int n_samples, n_features, n_classes;
    n_samples = 10;
    n_features = 5;
    n_classes = 1;
    string s;
    
    // the first line is 'X'
    freopen("../data/generated_10_2_5.txt", "r", stdin);

    // read in X train
    cin >> s;
    cout << s << endl;
    for (int i = 0; i < n_samples; i++) {
        vector<double> x;
        for (int j = 0; j < n_features; j++) {
            double temp;
            cin >> temp;
            x.push_back(temp);
        }
        x_train.push_back(x);
    }

    // read in Y train
    cin >> s;
    cout << s << endl;
    for (int i = 0; i < n_samples; i++) {
        vector<double> y;
        for (int j = 0; j < n_classes; j++) {
            double temp;
            cin >> temp;
            y.push_back(temp);
        }
        y_train.push_back(y);
    }

    // make an my_x_train and my_y_train including each index in the indices_to_read
    vector<vector<double>> my_x_train;
    vector<vector<double>> my_y_train;

    // loop through the indices_to_read and add the corresponding x_train and y_train to my_x_train and my_y_train
    for (int i = 0; i < indices_to_read.size(); i++) {
        int index = indices_to_read[i];
        my_x_train.push_back(x_train[index]);
        my_y_train.push_back(y_train[index]);
    }

    // print out the my_x_train and my_y_train's dimensions
    cout << "my_x_train size: " << my_x_train.size() << endl;
    cout << "my_y_train size: " << my_y_train.size() << endl;
}

int main() {
    n_samples = 10;
    n_features = 5;
    n_classes = 2;
    n_clients = 1;

    vector<vector<int>> indices = splitDataMaster(n_clients);
    

    vector<string> stringsToSend = getStringToSendClients(indices, n_clients);

    initialize_network();
    string network_init_string = network_string();


    // ENDED STRING INITIALIZATION
    cout << "Starting socket programming initialization" << endl;
    open_socket();

    bind_socket();
	
	map_sockets_to_ip_addrs();

	create_client_threads();

    join_threads();
    return 0;
}



