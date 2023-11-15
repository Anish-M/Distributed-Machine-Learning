#include "Layer.hpp"
#include <vector>

using namespace std;
using std::vector;

class ActivationLayer : public Layer {
private:
    vector<double> input;
    vector<double> output;
    std::function<double(double)> activation;
    std::function<double(double)> activation_prime;

public:
    // Constructor
    ActivationLayer(
        std::function<double(double)> activation_function,
        std::function<double(double)> activation_prime_function)
        : activation(activation_function), activation_prime(activation_prime_function) {}

    // Forward propagation
    vector<double> forward_propagation(const vector<double>& input_data) {
        this->input = input_data;
        this->output.clear();
        for (double i : input) {
            this->output.push_back(this->activation(i));
        }
        return this->output;
    }

    // Backward propagation
    vector<double> backward_propagation(const vector<double>& output_error) {
        vector<double> input_error;
        for (size_t i = 0; i < this->input.size(); ++i) {
            input_error.push_back(this->activation_prime(this->input[i]) * output_error[i]);
        }
        return input_error;
    }
};