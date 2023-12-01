#include <iostream>
#include <vector>

using namespace std;

class Layer {
public:
    vector<double> input;
    vector<double> output;

    // Computes the output Y of a layer for a given input X
    virtual vector<double> forwardPropagation(const vector<double>& input) = 0;

    // Computes dE/dX for a given dE/dY (and update parameters if any)
    virtual vector<double> backwardPropagation(const vector<double>& outputError, double learningRate) = 0;
};
