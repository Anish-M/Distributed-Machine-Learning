#include <vector>
#pragma once
using namespace std;

class Layer {
public:
    virtual vector<double> forward_propagation(const vector<double>& input) = 0;
    virtual vector<double> backward_propagation(const vector<double>& output_error, double learning_rate) = 0;
};