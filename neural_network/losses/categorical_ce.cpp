#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
using std::vector;


double categorical_ce(vector<double> y_true, vector<double> y_pred) {
    const double epsilon = 1e-15;

    // Clip y_pred values
    for (double& value : y_pred) {
        value = std::max(epsilon, std::min(1.0 - epsilon, value));
    }

    // Calculate cross-entropy loss
    double loss = 0.0;
    for (std::size_t i = 0; i < y_true.size(); ++i) {
        loss += y_true[i] * std::log(y_pred[i]);
    }

    return -loss;
}

vector<double> categorical_ce_prime(vector<double> y_true, vector<double> y_pred) {
    const double epsilon = 1e-15;

    // Clip y_pred values
    for (double& value : y_pred) {
        value = std::max(epsilon, std::min(1.0 - epsilon, value));
    }

    // Calculate cross-entropy loss
    vector<double> loss_prime;
    for (std::size_t i = 0; i < y_true.size(); ++i) {
        loss_prime.push_back(-y_true[i] / y_pred[i]);
    }

    return loss_prime;
}

int main () {
    vector<double> y_true;
    y_true.push_back(1);
    y_true.push_back(0);
    y_true.push_back(0);
    vector<double> y_pred;
    y_pred.push_back(0.7);
    y_pred.push_back(0.2);
    y_pred.push_back(0.1);
    std::cout << categorical_ce(y_true, y_pred) << std::endl;
    vector<double> output = categorical_ce_prime(y_true, y_pred);
    for (int i = 0; i < output.size(); i++) {
        printf("%f\n", output[i]);
    }
    return 0;
}