#include <functional>
#include <vector>
#include "layer.h"

using namespace std;

class ActivationLayer : public Layer {
private:
    // all the functions are in tehh form vector<double> softmax_prime(const vector<double>& x)
    function<vector<double>(const vector<double>&)> activation;
    function<vector<double>(const vector<double>&)> activation_prime;
    vector<double> input;
    vector<double> output;

public:
    ActivationLayer(function<vector<double>(const vector<double>&)> activation, function<vector<double>(const vector<double>&)> activation_prime) {
        this->activation = activation;
        this->activation_prime = activation_prime;
    }

    vector<double> forwardPropagation(const vector<double>& input) override {
        this->input = input;
        this->output.resize(input.size());

        this -> output = activation(input);
        return output;
    }
    

    vector<double> backwardPropagation(const vector<double>& outputError, double learningRate) override {
        // do this self.activation_prime(self.input) * output_error
        vector<double> inputError = activation_prime(input);
        for (size_t i = 0; i < inputError.size(); ++i) {
            inputError[i] *= outputError[i];
        }
        

        return inputError;
    }
};
