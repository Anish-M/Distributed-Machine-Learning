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

    vector<double> forward_propagation(const vector<double>& input) {
        vector<double> output(input.size());

        const int num_threads = thread::hardware_concurrency();

        auto forward_thread = [&](int thread_id) {
            for (size_t i = thread_id; i < input.size(); i += num_threads) {
                output[i] = activation(input[i]);
            }
        };

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