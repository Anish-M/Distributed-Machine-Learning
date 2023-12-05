#include <vector>
#include "Layer.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>

#pragma once

using namespace std;

class ActivationLayer : public Layer {
private:
    double (*activation)(double);
    double (*activation_prime)(double);
    string functionName;

public:
    ActivationLayer(double (*activation)(double), double (*activation_prime)(double), string functionName)
        : activation(activation), activation_prime(activation_prime), functionName(functionName) {}

    vector<double> forward_propagation(const vector<double>& input) override {
        vector<double> output;
        for (double i : input) {
            output.push_back(activation(i));
        }
        return output;
    }


    // vector<double> forward_propagation(const vector<double>& input) {
    //     vector<double> output(input.size());

    //     const int num_threads = 64;

    //     vector<thread> threads;

    //     // split the work based on the size of input and number of threads
    //     size_t output_size = input.size();

    //     // Calculate the number of elements each thread should process
    //     size_t chunk_size = output_size / num_threads;


    //     for (size_t i = 0; i < num_threads; ++i) {
    //         size_t start = i * chunk_size;
    //         size_t end = (i == num_threads - 1) ? output_size : (i + 1) * chunk_size;

    //         threads.emplace_back([&, start, end]() {
    //             for (size_t j = start; j < end; ++j) {
    //                 output[j] = activation(input[j]);
    //             }
    //         });
    //     }


    //     // Join threads
    //     for (auto& thread : threads) {
    //         thread.join();
    //     }

    //     return output;
    // }


    vector<double> backward_propagation(const vector<double>& output_error, double learning_rate) override {
        vector<double> input_error;
        for (size_t i = 0; i < output_error.size(); ++i) {
            input_error.push_back(activation_prime(output_error[i]) * output_error[i]);
        }
        return input_error;
    }
    string getFunctionName() {
        return functionName;
    }
};