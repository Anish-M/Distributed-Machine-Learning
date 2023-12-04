#include <vector>
#include <ctime>
#include "Layer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "FCLayer.hpp"
#include "ActivationLayer.hpp"
#include <map>
#include <utility>
#include <sstream>
using namespace std;

using pair_type = pair<vector<vector<vector<double>>>, vector<vector<double>>>;

class Network {
private:
    vector<Layer*> layers;
    double (*loss)(const vector<double>&, const vector<double>&);
    vector<double> (*loss_prime)(const vector<double>&, const vector<double>&);

public:
    Network() : loss(nullptr), loss_prime(nullptr) {}

    void add(Layer* layer) {
        layers.push_back(layer);
    }

    void use(double (*loss_function)(const vector<double>&, const vector<double>&),
             vector<double> (*loss_prime_function)(const vector<double>&, const vector<double>&)) {
        loss = loss_function;
        loss_prime = loss_prime_function;
    }

    vector<double> predict(const vector<double>& input_data) {
        vector<double> result = input_data;
        for (Layer* layer : layers) {
            result = layer->forward_propagation(result);
        }
        return result;
    }

    void fit(const vector<vector<double>>& x_train, const vector<vector<double>>& y_train, int epochs, double learning_rate) {
        for (int i = 0; i < epochs; ++i) {
            double err = 0;
            auto start_time_epoch = clock();
            for (size_t j = 0; j < x_train.size(); ++j) {
                auto start_time = clock();
                vector<double> output = x_train[j];
                for (Layer* layer : layers) {
                    output = layer->forward_propagation(output);
                }

                err += loss(y_train[j], output);

                vector<double> error = loss_prime(y_train[j], output);
                for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
                    error = (*it)->backward_propagation(error, learning_rate);
                }

                // cout << "Epoch " << i + 1 << "/" << epochs << " Sample " << j + 1 << "/" << x_train.size()
                //      << " Time: " << double(clock() - start_time) / CLOCKS_PER_SEC << "s" << endl;
            }
            err /= x_train.size();
            double time_for_epoch = double(clock() - start_time_epoch) / CLOCKS_PER_SEC;
            cout << "Epoch " << i + 1 << "/" << epochs << " error=" << err
                 << " time for epoch=" << time_for_epoch << "s" << endl;
        }
    }

    // data parallelism
    void fitOneEpoch(const vector<vector<double>>& x_train, const vector<vector<double>>& y_train, double learning_rate) {
        double err = 0;
        auto start_time_epoch = clock();
        for (size_t j = 0; j < x_train.size(); ++j) {
            auto start_time = clock();
            vector<double> output = x_train[j];
            for (Layer* layer : layers) {
                output = layer->forward_propagation(output);
            }

            err += loss(y_train[j], output);

            vector<double> error = loss_prime(y_train[j], output);
            for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
                error = (*it)->backward_propagation(error, learning_rate);
            }


            // cout << "Epoch " << i + 1 << "/" << epochs << " Sample " << j + 1 << "/" << x_train.size()
            //      << " Time: " << double(clock() - start_time) / CLOCKS_PER_SEC << "s" << endl;
        }
        err /= x_train.size();
        double time_for_epoch = double(clock() - start_time_epoch) / CLOCKS_PER_SEC;
        cout << "error: " << err << " time for epoch=" << time_for_epoch << "s" << endl;
    }

    // model parallelism
    vector<double> fit_model_forwardProp(const vector<double>& x_train, const vector<double>& y_train, int epochs, double learning_rate, int my_layer) {
    
        double err = 0;
        auto start_time_epoch = clock();
        
        auto start_time = clock();
        vector<double> output = x_train;

        int count = 0;
        for (Layer* layer : layers) {
            if(count == my_layer) {
                output = layer->forward_propagation(output);
                break;
            }

            count++;
        }

        err += loss(y_train, output);



        return output;
    }


    void fit_model_backProp(const vector<double>& x_train, const vector<double>& y_train, vector<double>& error, int epochs, double learning_rate, int my_layer, const vector<vector<double>>& output) {
       
        int count = 0;
        for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
            if(count == my_layer) {
                error = (*it)->backward_propagation(error, learning_rate);
                return;
            }
            count++;
        }

        // double time_for_epoch = double(clock() - start_time_epoch) / CLOCKS_PER_SEC;
    }

    string getWeights() {
        string weights = "weights:\n";
        for (Layer* layer : layers) {
            FCLayer* fc_layer = dynamic_cast<FCLayer*>(layer);
            ActivationLayer* activation_layer = dynamic_cast<ActivationLayer*>(layer);
            if (fc_layer != nullptr) {
                vector<vector<double>> layer_weights = fc_layer->getWeights();
                for (int i = 0; i < layer_weights.size(); i++) {
                    for (int j = 0; j < layer_weights[i].size(); j++) {
                        weights += to_string(layer_weights[i][j]) + " ";
                    }
                    weights += "\n";
                }
            } else {
                weights += activation_layer->getFunctionName() + "\n";
            }
            weights += "\n";
        }
        return weights;
    }

    string getBiases() {
        string biases = "biases:\n";
        for (Layer* layer : layers) {
            FCLayer* fc_layer = dynamic_cast<FCLayer*>(layer);
            ActivationLayer* activation_layer = dynamic_cast<ActivationLayer*>(layer);
            if (fc_layer != nullptr && activation_layer == nullptr) {
                vector<double> layer_biases = fc_layer->getBiases();
                for (int i = 0; i < layer_biases.size(); i++) {
                    biases += to_string(layer_biases[i]) + " ";
                }
                biases += "\n";
            }
            // biases += "\n";
        }
        return biases;
    }

    string network_string_init () {
        string network_str = "INITSTART\n";
        network_str += getWeights();
        network_str += "\n";
        network_str += getBiases();
        network_str += "\n";
        network_str += "mse\n";
        network_str += "INITEND";
        return network_str;
    }

    string network_string () {
        string network_str = "";
        network_str += getWeights();
        network_str += "\n";
        network_str += getBiases();
        network_str += "\n";
        network_str += "mse\n";
        return network_str;
    }

    
    pair_type handle_worker_network_message(string message) {
        istringstream stream(message);
        vector<vector<vector<double>>> weights;
        vector<vector<double>> biases;
        vector<string> activations;

        string line;
        bool inWeightsSection = false, inBiasesSection = false;
        while (getline(stream, line)) {
            // Check for section headers
            if (line.find("DATAP_WORKER_START") != string::npos) {
                continue;
            }
            if (line.find("DATAP_WORKER_END") != string::npos) {
                break;
            }
            if (line.find("weights:") != string::npos) {
                inWeightsSection = true;
                inBiasesSection = false;
                weights.push_back(vector<vector<double>>());
                continue;
            }
            if (line.find("biases:") != string::npos) {
                inWeightsSection = false;
                inBiasesSection = true;
                continue;
            }
            // Check for activation functions
            if (line.find("tanh") != string::npos || line.find("relu") != string::npos || 
                line.find("sigmoid") != string::npos || line.find("mse") != string::npos) {
                activations.push_back(line);
                weights.push_back(vector<vector<double>>());
                continue;
            }

            // Parse numerical data
            istringstream linestream(line);
            vector<double> numbers;
            double num;
            while (linestream >> num) {
                numbers.push_back(num);
            }

            // Store data in corresponding section
            if (inWeightsSection && !numbers.empty()) {
                weights.back().push_back(numbers);
            } else if (inBiasesSection && !numbers.empty()) {
                biases.push_back(numbers);
            }
        }
        weights.pop_back();
        weights.pop_back();
        return make_pair(weights, biases);
    }

    void masterReadInNetwork(map<string, string> network) {
        // readInNetwork String, writing values instead of overwriting
        // zero out current weights and biases
        // for (Layer* layer : layers) {
        //     FCLayer* fc_layer = dynamic_cast<FCLayer*>(layer);
        //     ActivationLayer* activation_layer = dynamic_cast<ActivationLayer*>(layer);
        //     if (fc_layer != nullptr) {
        //         vector<vector<double>> layer_weights = fc_layer->getWeights();
                
        //     }
        // }
        // print the length of each value
        // print out the map
        cout << "Master Read In Network" << endl;
        vector<pair_type> all_network_data;
        for (auto const& x : network) {
            pair_type this_worker = handle_worker_network_message(x.second);
            all_network_data.push_back(this_worker);
        }
        // cout << endl;
        if (all_network_data.empty()) {
            throw runtime_error("No network data available.");
        }

        int num_networks = all_network_data.size();
        vector<vector<vector<double>>> avg_weights = all_network_data[0].first;
        vector<vector<double>> avg_biases = all_network_data[0].second;

        for (int i = 1; i < num_networks; ++i) {
            for (size_t layer = 0; layer < avg_weights.size(); ++layer) {
                for (size_t neuron = 0; neuron < avg_weights[layer].size(); ++neuron) {
                    for (size_t weight = 0; weight < avg_weights[layer][neuron].size(); ++weight) {
                        avg_weights[layer][neuron][weight] += all_network_data[i].first[layer][neuron][weight];
                    }
                }

                for (size_t bias = 0; bias < avg_biases[layer].size(); ++bias) {
                    avg_biases[layer][bias] += all_network_data[i].second[layer][bias];
                }
            }
        }

        for (auto& layer : avg_weights) {
            for (auto& neuron : layer) {
                for (auto& weight : neuron) {
                    weight /= num_networks;
                }
            }
        }

        for (auto& layer : avg_biases) {
            for (auto& bias : layer) {
                bias /= num_networks;
            }
        }

        // loop through the layers and set the weights and biases
        int i = 0;
        for (Layer* layer : layers) {
            FCLayer* fc_layer = dynamic_cast<FCLayer*>(layer);
            ActivationLayer* activation_layer = dynamic_cast<ActivationLayer*>(layer);
            if (fc_layer != nullptr) {
                fc_layer->setWeights(avg_weights[i]);
                fc_layer->setBiases(avg_biases[i]);
                i++;
            }
        }
    }
};