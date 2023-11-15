#include "relu.h"
#include <stdio.h>
#include <vector>
#include <math.h>
using std::vector;

vector<double> sigmoid(vector<double> input) {
    vector<double> output;
    for (int i = 0; i < input.size(); i++) {
        output.push_back(1 / (1 + exp(-input[i])));
    }
    return output;

}

vector<double> sigmoid_prime(vector<double> input) {
    vector<double> output;
    for (int i = 0; i < input.size(); i++) {
        output.push_back(exp(-input[i]) / pow((1 + exp(-input[i])), 2));
    }
    return output;
}

// write main class to test this
int main() {
    vector<double> input;
    input.push_back(1);
    input.push_back(2);
    input.push_back(3);
    vector<double> output = sigmoid(input);
    for (int i = 0; i < output.size(); i++) {
        printf("%f\n", output[i]);
    }
    return 0;
}