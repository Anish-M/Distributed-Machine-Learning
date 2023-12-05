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

    // vector<double> forward_propagation(const vector<double>& input) override {
    //     vector<double> output;
    //     for (double i : input) {
    //         output.push_back(activation(i));
    //     }
    //     return output;
    // }


    vector<double> forward_propagation(const vector<double>& input) {
        vector<double> output(input.size());

        const int num_threads = 8;

        // split the work based on the size of input and number of threads
        const int chunk_size = input.size() / num_threads;
        for (int i = 0; i < num_threads; ++i) {
            int start = i * chunk_size;
            int end = (i + 1) * chunk_size;
            if (i == num_threads - 1) {
                end = input.size();
            }
            threads.emplace_back(forward_thread, i, ref(input), ref(output), start, end);
        }

        vector<thread> threads;

        // Launch threads
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(forward_thread, i);
        }

        // Join threads
        for (auto& thread : threads) {
            thread.join();
        }

        return output;
    }


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