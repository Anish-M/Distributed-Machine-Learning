#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using std::vector;

class FCLayer {
private:
    vector<vector<double>> weights;
    vector<vector<double>> bias;
    vector<vector<double>> input;
    vector<vector<double>> output;

public:
    FCLayer(int input_size, int output_size) {
        // Initialize random seed
        std::srand(std::time(0));

        // Initialize weights and bias with random values between -0.5 and 0.5
        weights.resize(input_size, vector<double>(output_size));
        bias.resize(1, vector<double>(output_size));

        for (int i = 0; i < input_size; ++i) {
            for (int j = 0; j < output_size; ++j) {
                weights[i][j] = static_cast<double>(rand()) / RAND_MAX - 0.5;
            }
        }

        for (int j = 0; j < output_size; ++j) {
            bias[0][j] = static_cast<double>(rand()) / RAND_MAX - 0.5;
        }
    }

    vector<vector<double>> forwardPropagation(const vector<vector<double>>& input_data) {
        input = input_data;

        output.resize(input.size(), vector<double>(bias[0].size(), 0.0));

        for (size_t i = 0; i < input.size(); ++i) {
            for (size_t j = 0; j < bias[0].size(); ++j) {
                output[i][j] = 0.0;
                for (size_t k = 0; k < weights.size(); ++k) {
                    output[i][j] += input[i][k] * weights[k][j];
                }
                output[i][j] += bias[0][j];
            }
        }

        return output;
    }

    vector<vector<double>> backwardPropagation(const vector<vector<double>>& output_error, double learning_rate) {
        vector<vector<double>> input_error(input.size(), vector<double>(weights[0].size(), 0.0));

        for (size_t i = 0; i < input.size(); ++i) {
            for (size_t j = 0; j < weights[0].size(); ++j) {
                input_error[i][j] = 0.0;
                for (size_t k = 0; k < output_error[0].size(); ++k) {
                    input_error[i][j] += output_error[i][k] * weights[j][k];
                }
            }
        }

        for (size_t i = 0; i < weights.size(); ++i) {
            for (size_t j = 0; j < weights[0].size(); ++j) {
                double weights_error = 0.0;
                for (size_t k = 0; k < input.size(); ++k) {
                    weights_error += input[k][i] * output_error[k][j];
                }
                weights[i][j] -= learning_rate * weights_error;
            }
        }

        for (size_t j = 0; j < bias[0].size(); ++j) {
            double bias_error = 0.0;
            for (size_t i = 0; i < input.size(); ++i) {
                bias_error += output_error[i][j];
            }
            bias[0][j] -= learning_rate * bias_error;
        }

        return input_error;
    }
};

int main() {
    // Example usage
    int input_size = 3;
    int output_size = 2;

    FCLayer fcLayer(input_size, output_size);

    // Example input data
    vector<vector<double>> input_data = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};

    // Forward propagation
    vector<vector<double>> output_data = fcLayer.forwardPropagation(input_data);

    printf("Output data:\n");
    for (size_t i = 0; i < output_data.size(); ++i) {
        for (size_t j = 0; j < output_data[0].size(); ++j) {
            printf("%f ", output_data[i][j]);
        }
        printf("\n");
    }

    // Example output error
    vector<vector<double>> output_error = {{0.1, 0.2}, {0.3, 0.4}};

    // Backward propagation
    vector<vector<double>> input_error = fcLayer.backwardPropagation(output_error, 0.01);

    printf("Input error:\n");
    for (size_t i = 0; i < input_error.size(); ++i) {
        for (size_t j = 0; j < input_error[0].size(); ++j) {
            printf("%f ", input_error[i][j]);
        }
        printf("\n");
    }

    return 0;
}
