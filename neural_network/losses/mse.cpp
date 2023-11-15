#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
using std::vector;



double mse(vector<double> y_true, vector<double> y_pred) {
    double output = 0;
    for (int i = 0; i < y_true.size(); i++) {
        output += pow(y_true[i] - y_pred[i], 2);
    }
    return output / y_true.size();
}

vector<double> mse_prime(vector<double> y_true, vector<double> y_pred) {
    vector<double> output;
    for (int i = 0; i < y_true.size(); i++) {
        output.push_back(2 * (y_pred[i] - y_true[i]) / y_true.size());
    }
    return output;
}

int main() {
    vector<double> y_true;
    y_true.push_back(1);
    y_true.push_back(2);
    y_true.push_back(3);
    vector<double> y_pred;
    y_pred.push_back(1);
    y_pred.push_back(2);
    y_pred.push_back(5);
    std::cout << mse(y_true, y_pred) << std::endl;
    vector<double> output = mse_prime(y_true, y_pred);
    for (int i = 0; i < output.size(); i++) {
        printf("%f\n", output[i]);
    }
    return 0;
}