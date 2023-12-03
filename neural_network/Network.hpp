#include <vector>
#include <ctime>
#include "Layer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "FCLayer.hpp"
#include "ActivationLayer.hpp"

using namespace std;

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
        cout << " time for epoch=" << time_for_epoch << "s" << endl;
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
};