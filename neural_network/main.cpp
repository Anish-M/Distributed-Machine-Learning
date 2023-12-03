#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <ctime>
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

using namespace std;

// ################ SOCKET PROGRAMMING ATTRIBUTES ########################
int sockfd, portno;
socklen_t clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;

vector<int> newsockfds;
vector<thread> threads;

// mapping of sockets to ip addresses
vector<string> ip_addrs;

vector<thread> client_threads;

int n_clients = 1;
// ######################################################

int main() {
    // Example usage
    Network network;

    vector<vector<double>> x_train;
    vector<vector<double>> y_train;

    // Read x_train and y_train from file data/generated8000_2_16000.txt
    // n_samples/n_features/n_classes
    // // freopen("../data/generated8000_2_16000.txt", "r", stdin);
    int n_samples, n_features, n_classes;
    n_samples = 8000;
    n_features = 16000;
    n_classes = 2;

    // Read x_train the first line of the file is 'X' folllowed by n_samples lines of n_features
    string s;
    
    // the first line is 'X'
    // open the file ../data/generated8000_2_16000.txt with cin
    freopen("../data/generated_8000_2_16000.txt", "r", stdin);
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


    network.add(new FCLayer(n_features, 18000));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(18000, 20000));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(20000, 25000));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(25000, 17000));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(17000, 8000));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(8000, 3000));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(3000, 1000));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(1000, 500));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(500, 1));  // Add fully connected layer with 3 inputs and 4 outputs


    network.use(mse, mse_prime);     // Use mean squared error loss
    network.fit(x_train, y_train, 10, 0.001);
    return 0;
}
