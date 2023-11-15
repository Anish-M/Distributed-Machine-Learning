#include "relu.h"
#include <stdio.h>
#include <vector>

using std::vector;

vector<double> relu(vector<double> input) {
    vector<double> output;
    for (int i = 0; i < input.size(); i++) {
        if (input[i] > 0) {
            output.push_back(input[i]);
        } else {
            output.push_back(0);
        }
    }
    return output;
}

vector<double> relu_prime(vector<double> input) {
    vector<double> output;
    for (int i = 0; i < input.size(); i++) {
        if (input[i] > 0) {
            output.push_back(1);
        } else {
            output.push_back(0);
        }
    }
    return output;
}

int main() {
    vector<double> input;
    input.push_back(-1);
    input.push_back(2);
    input.push_back(3);
    vector<double> output = relu(input);
    for (int i = 0; i < output.size(); i++) {
        printf("%f\n", output[i]);
    }
    return 0;
}