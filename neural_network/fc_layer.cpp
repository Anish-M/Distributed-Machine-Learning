#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "layer.h"
using namespace std;


class FCLayer : public Layer {
public:
    vector<vector<double>> weights;
    vector<vector<double>> bias;

    FCLayer(int inputSize, int outputSize) {
        weights.resize(inputSize, vector<double>(outputSize));
        bias.resize(1, vector<double>(outputSize));

        // Initialize weights and bias with random values between 0 and1
        srand(time(0));
        for (int i = 0; i < inputSize; ++i) {
            for (int j = 0; j < outputSize; ++j) {
                weights[i][j] = static_cast<double>(rand()) / RAND_MAX;
            }
        }
        for (int i = 0; i < outputSize; ++i) {
            bias[0][i] = static_cast<double>(rand()) / RAND_MAX;
        }

    }

    vector<double> forwardPropagation(const vector<double>& input) override {
        input_ = input;

        // Matrix multiplication and addition (dot product with bias)
        output_ = vector<double>(bias[0].size(), 0.0);
        for (size_t i = 0; i < weights.size(); ++i) {
            for (size_t j = 0; j < weights[0].size(); ++j) {
                output_[j] += input_[i] * weights[i][j];
            }
        }
        for (size_t i = 0; i < output_.size(); ++i) {
            output_[i] += bias[0][i];
        }

        return output_;
    }

    vector<double> backwardPropagation(const vector<double>& outputError, double learningRate) override {
        // Compute inputError (dE/dX)
        vector<double> inputError(weights[0].size(), 0.0);
        for (size_t i = 0; i < weights.size(); ++i) {
            for (size_t j = 0; j < weights[0].size(); ++j) {
                inputError[i] += outputError[j] * weights[i][j];
            }
        }

        // Compute weightsError (dE/dW) and update weights
        for (size_t i = 0; i < weights.size(); ++i) {
            for (size_t j = 0; j < weights[0].size(); ++j) {
                weights[i][j] -= learningRate * (input_[i] * outputError[j]);
            }
        }

        // Compute biasError (dE/dB) and update bias
        for (size_t i = 0; i < bias[0].size(); ++i) {
            bias[0][i] -= learningRate * outputError[i];
        }

        return inputError;
    }

private:
    vector<double> input_;
    vector<double> output_;
};
