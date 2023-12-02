#include "Layer.hpp"
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

class FCLayer : public Layer {
private:
    vector<vector<double>> weights;
    vector<double> bias;

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

    vector<double> forward_propagation(const vector<double>& input) override {
        vector<double> output;
        for (size_t i = 0; i < bias.size(); ++i) {
            double activation = bias[i];
            for (size_t j = 0; j < input.size(); ++j) {
                activation += input[j] * weights[j][i];
            }
            output.push_back(activation);
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
};
