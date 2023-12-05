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
#include "../sockets/master.cpp"
#include <chrono>
#include <map>
#include <iomanip> 


using namespace std;



// ################# NEURAL NETWORK ATTRIBUTES ############################
int n_samples, n_features, n_classes;
int epochs;
vector<vector<double>> x_train;
vector<vector<double>> y_train;
Network network;
// ########################################################################
// ################ NETWORK COMMUNICATION ATTRIBUTES ######################
map<string, string> workerReplies;
map<string, bool> ipToWorkInProgress;
map<string, int> ipToWorkDone;
map<string, int> ipToClientIndex;
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


    network.add(new FCLayer(n_features, 100));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime, "tanh_1"));
    network.add(new FCLayer(100, 80));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(relu, relu_prime, "relu")); // Add activation layer with sigmoid
    network.add(new FCLayer(80, 40));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(relu, relu_prime, "relu")); // Add activation layer with sigmoid
    network.add(new FCLayer(40, 20));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime, "tanh")); // Add activation layer with sigmoid
    network.add(new FCLayer(20, 2));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime, "tanh")); // Add activation layer with sigmoid
    network.use(mse, mse_prime);
    

    return 0;
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
    cout << "y_train sizef " << y_train.size() << endl;

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

    return chunks;
}



vector<string> getStringToSendClients(vector<vector<int>> indices, int numWorkers) {
    vector<string> stringsToSend;
    for (int i = 0; i < numWorkers; i++) {
        string s = "WORKINDEXSTART";
        for (int j = 0; j < indices[i].size(); j++) {
            s += to_string(indices[i][j]) + ",";
        }
        s += "WORKINDEXEND";
        stringsToSend.push_back(s);
    }
    return stringsToSend;
}



// receiving the network back from the clients
void handle_message_from_worker(string message, string ip_addr) {
    // message structure: DATAP_WORKER_START ......... DATAP_WORKER_END in sending the network
    if (message.find("DATAP_WORKER_END") != string::npos)
    { 
        ipToWorkInProgress[ip_addr] = false;
        workerReplies[ip_addr] += message;
        ipToWorkDone[ip_addr] = true;
        // just print out message
        
    }
    else if (message.find("DATAP_WORKER_START") != string::npos)
    {
        workerReplies[ip_addr] = "";
        workerReplies[ip_addr] += message;
        ipToWorkInProgress[ip_addr] = true;

    }
    else if (message.find("DATAP_WORKER_RESEND") != string::npos)
    {
        cout << "received restart message from " << ip_addr << endl;
        ipToWorkInProgress[ip_addr] = true;
        ipToWorkDone[ip_addr] = false;
        string msg = "DATAP_WORKER_RESTART\n" + network.network_string() + "INITEND";
        send_message_to_client(ipToClientIndex[ip_addr], msg);
    }
    else if (ipToWorkInProgress[ip_addr] == true)
    {
        workerReplies[ip_addr] += message;
    }
}
void create_client_threads() {
	// now all the client connections have been received, create a thread for each client to continuously read from the socket
	for (int i = 0; i < n_clients; i++) {
		printf("Creating thread for client %s\n", ip_addrs[i].c_str());
		client_threads.push_back(thread([](int newsockfd, string ip_addr) {
			char buffer[256];
			int n;
			while (true || buffer[0] != 113) {
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255);
				if (n < 0) {
					perror("perror reading from socket");
				} else {
                    string msg(buffer);
                    handle_message_from_worker(msg, ip_addr.c_str());
					if (buffer[0] == 113) {
						printf("buffer[0] = %d\n", buffer[0]);
						printf("Client %s has disconnected\n", ip_addr.c_str());
						break;
					}
				}
			}
			printf("Thread exiting for client %s\n", ip_addr.c_str());
		}, newsockfds[i], ip_addrs[i]));
	}
}

bool all_completed_this_epoch() {
    for (auto it = ipToWorkDone.begin(); it != ipToWorkDone.end(); it++) {
        if (it->second == false) {
            return false;
        }
    }
    return true;
}

int main() {
    n_samples = 10000;
    n_features = 60;
    n_classes = 2;
    n_clients = 1;
    epochs = 20;

    vector<vector<int>> indices = splitDataMaster(n_clients);
    

    vector<string> stringsToSend = getStringToSendClients(indices, n_clients);
    // print out stringsToSend

    initialize_network();
    string network_init_string = network.network_string_init();


    // ENDED STRING INITIALIZATION

    cout << "----------------------------------------" << endl;
    cout << "Starting socket programming initialization" << endl;
    open_socket();

    bind_socket();
	
	map_sockets_to_ip_addrs();

	create_client_threads();

    // INITIALIZE THE MAPS ipToWorkInProgress and ipToWorkDone
    for (int i = 0; i < n_clients; i++) {
        ipToWorkInProgress[ip_addrs[i].c_str()] = false;
        ipToClientIndex[ip_addrs[i].c_str()] = i;
    }
    for (int i = 0; i < n_clients; i++) {
        ipToWorkDone[ip_addrs[i].c_str()] = false;
    }

    cout << "Finished socket programming initialization" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Sending messages for network initialization" << endl;

    // send_thread the network_init_string to all the clients
    for (int i = 0; i < n_clients; i++) {
        send_message_to_client(i, network_init_string);
        sleep(0.1);
    }


    cout << "Finished sending messages for network initialization" << endl;
    cout << "----------------------------------------" << endl;

    cout << "Sending the work indices to the clients" << endl;
    for (int i = 0 ; i < 4; i++) {
        for (int i = 0; i < n_clients; i++) {
            send_message_to_client(i, stringsToSend[i]);
            sleep(0.1);
        }
    }
    cout << "Finished sending the work indices to the clients" << endl;
    cout << "----------------------------------------" << endl;


    // this is for DATA PARALLELISM
    cout << "Starting network training..." << endl;

    // measure the time it takes to train the network
    auto start = chrono::high_resolution_clock::now();
    for(int x = 1; x <= epochs; x++) {
        /// wait for all the epochs
        cout << "----------------------------------------" << endl;
        cout << "Waiting for all the workers to finish epoch..." << x << endl;
        for (auto it = ipToWorkDone.begin(); it != ipToWorkDone.end(); it++) {
            cout << it->first << " " << it->second << endl;
        }
        
        auto start_timeout = chrono::high_resolution_clock::now();
        while (all_completed_this_epoch() == false) {
            sleep(0.2);
            // set a timeout for 10 seconds
            auto end_timeout = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::seconds>(end_timeout - start_timeout);
            if (duration.count() > 5 / (n_clients + 1)) {
                // U DIDNT RECEIVE SOMETHING FROM SOME WORKER
                // DO THE LOGIC HERE
                // title the message "RESEND"
                // send the message to the worker
                for (auto it = ipToWorkDone.begin(); it != ipToWorkDone.end(); it++) {
                    if (it->second == false) {
                        send_message_to_client(ipToClientIndex[it->first], "WORKER_RESEND");
                    }
                }
                start_timeout = chrono::high_resolution_clock::now();
            }
        }
        cout << "All workers finished epoch..." << x << endl;
        network.masterReadInNetwork(workerReplies);

        string sendNewNet = network.network_string_init();
        // set the ipToWorkDone to false
        for (auto& it : ipToWorkDone) {
            it.second = false;
        }

        for (int i = 0; i < n_clients; i++) {
            send_message_to_client(i, sendNewNet);
        }

        workerReplies = {};
        cout << "----------------------------------------" << endl;
    }
    auto end = chrono::high_resolution_clock::now();
    double time_taken = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << "Time taken for training is: " << time_taken << " seconds" << endl;
    join_threads();
    return 0;
}



