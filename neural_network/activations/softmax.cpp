#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

using std::vector;
// take the max of 0 and the input which is a vector of doubles
vector<double> softmax(vector<double> input) {
    vector<double> output;
    double sum = 0;
    for (int i = 0; i < input.size(); i++) {
        sum += exp(input[i]);
    }
    for (int i = 0; i < input.size(); i++) {
        output.push_back(exp(input[i]) / sum);
    }
    return output;
}

vector<double> softmax_prime(vector<double> input) {
    vector<double> output;
    double sum = 0;
    for (int i = 0; i < input.size(); i++) {
        sum += exp(input[i]);
    }
    for (int i = 0; i < input.size(); i++) {
        output.push_back(exp(input[i]) / sum);
    }
    return output;
}

int main() {
    vector<double> input;
    input.push_back(1);
    input.push_back(2);
    input.push_back(3);
    vector<double> output = softmax(input);
    for (int i = 0; i < output.size(); i++) {
        std::cout << output[i] << std::endl;
    }
    return 0;
}