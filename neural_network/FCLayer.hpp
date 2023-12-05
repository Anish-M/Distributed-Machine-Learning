#include <vector>
#include <numeric>
#include <algorithm>
#include <thread>
#include <iostream>
#include <chrono>
#include <random>
#include <functional>
#pragma once

using namespace std;

class FCLayer : public Layer {
private:
    vector<vector<double>> weights;
    vector<double> bias;

        
    vector<double> calculate_partial_output(const vector<double>& input, const vector<vector<double>>& weights, const vector<double>& bias, size_t start, size_t end) {
        vector<double> partial_output;
        for (size_t i = start; i < end; ++i) {
            double activation = bias[i];
            for (size_t j = 0; j < input.size(); ++j) {
                activation += input[j] * weights[j][i];
            }
            partial_output.push_back(activation);
        }
        return partial_output;
    }

public:
    FCLayer(size_t input_size, size_t output_size) {
        weights.resize(input_size, vector<double>(output_size));
        bias.resize(output_size);
        // Initialize weights and biases
        for (auto& w : weights)
            for (double& val : w)
                val = (rand() / double(RAND_MAX)) - 0.5;
        for (double& b : bias)
            b = (rand() / double(RAND_MAX)) - 0.5;
    }

    FCLayer(const vector<vector<double>>& weights, const vector<double>& bias) : weights(weights), bias(bias) {}


    // vector<double> forward_propagation(const vector<double>& input) override {
    //     vector<double> output;
    //     for (size_t i = 0; i < bias.size(); ++i) {
    //         double activation = bias[i];
    //         for (size_t j = 0; j < input.size(); ++j) {
    //             activation += input[j] * weights[j][i];
    //         }
    //         output.push_back(activation);
    //     }
    //     return output;
    // }



    vector<double> forward_propagation(const vector<double>& input) {
        // Get the number of threads supported by the hardware
        const int num_threads = 8;

        size_t output_size = bias.size();
        vector<double> output(output_size, 0.0);

        // Calculate the number of elements each thread should process
        size_t chunk_size = output_size / num_threads;

        // Vector to store thread objects
        vector<thread> threads;

        // Launch threads
        for (size_t i = 0; i < num_threads; ++i) {
            size_t start = i * chunk_size;
            size_t end = (i == num_threads - 1) ? output_size : (i + 1) * chunk_size;

            threads.emplace_back([&, start, end]() {
                auto partial_output = calculate_partial_output(input, weights, bias, start, end);
                
                // Update the global output vector with the partial result
                for (size_t j = start; j < end; ++j) {
                    output[j] = partial_output[j - start];
                }
            });
        }

        // Join threads
        for (auto& thread : threads) {
            thread.join();
        }

        return output;
    }

    vector<double> backward_propagation(const vector<double>& output_error, double learning_rate) override {
        vector<double> input_error(weights.size());
        for (size_t i = 0; i < input_error.size(); ++i) {
            for (size_t j = 0; j < output_error.size(); ++j) {
                input_error[i] += output_error[j] * weights[i][j];
                weights[i][j] -= learning_rate * output_error[j];
            }
        }
        for (size_t i = 0; i < bias.size(); ++i) {
            bias[i] -= learning_rate * output_error[i];
        }
        return input_error;
    }

    vector<vector<double>> getWeights() {
        return weights;
    }
    vector<double> getBiases() {
        return bias;
    }

    void setWeights(vector<vector<double>> newWeights) {
        weights = newWeights;
    }
    
    void setBiases(vector<double> newBiases) {
        bias = newBiases;
    }
    // void zeroWeightsBiases(){
    //     std::fill(weights.begin(), weights.end(), 0);
    // }
};
