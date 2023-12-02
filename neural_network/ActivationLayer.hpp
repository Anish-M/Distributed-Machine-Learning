#include <vector>
#include "Layer.hpp"

using namespace std;

class ActivationLayer : public Layer {
private:
    double (*activation)(double);
    double (*activation_prime)(double);

public:
    ActivationLayer(double (*activation)(double), double (*activation_prime)(double))
        : activation(activation), activation_prime(activation_prime) {}

    vector<double> forward_propagation(const vector<double>& input) override {
        vector<double> output;
        for (double i : input) {
            output.push_back(activation(i));
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
};