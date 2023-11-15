#include "Layer.hpp"
#include <vector>

using namespace std;

class ActivationLayer : public Layer {
private:
    std::vector<double> input;
    std::vector<double> output;
    std::function<double(double)> activation;
    std::function<double(double)> activation_prime;

public:
    // Constructor
    ActivationLayer(
        std::function<double(double)> activation_function,
        std::function<double(double)> activation_prime_function)
        : activation(activation_function), activation_prime(activation_prime_function) {}

    // Forward propagation
    std::vector<double> forward_propagation(const std::vector<double>& input_data) {
        this->input = input_data;
        this->output.clear();
        for (double i : input) {
            this->output.push_back(this->activation(i));
        }
        return this->output;
    }

    // Backward propagation
    std::vector<double> backward_propagation(const std::vector<double>& output_error) {
        std::vector<double> input_error;
        for (size_t i = 0; i < this->input.size(); ++i) {
            input_error.push_back(this->activation_prime(this->input[i]) * output_error[i]);
        }
        return input_error;
    }
};