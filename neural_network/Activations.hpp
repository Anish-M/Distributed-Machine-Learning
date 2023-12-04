#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <ctime>

using namespace std;

// Activation functions and their derivatives
double tanh_1(double x) {
    return tanh(x);
}

double tanh_prime(double x) {
    double tx = std::tanh(x);
    return 1 - tx * tx;
}

// Softmax should be applied to a vector, not a single value. Placeholder function.
double softmax(double x) {
    return exp(x);
}

double sigmoid(double z) {
    return 1 / (1 + exp(-z));
}

double sigmoid_prime(double z) {
    double sz = sigmoid(z);
    return sz * (1 - sz);
}

double relu(double x) {
    return max(0.0, x);
}

double relu_prime(double x) {
    return x <= 0 ? 0 : 1;
}

double leaky_relu(double x) {
    double alpha = 0.01;
    return x >= 0 ? x : alpha * x;
}

double leaky_relu_prime(double x) {
    double alpha = 0.01;
    return x >= 0 ? 1 : alpha;
}

// Loss functions
double mse(const vector<double>& y_true, const vector<double>& y_pred) {
    double sum = 0;
    for (size_t i = 0; i < y_true.size(); ++i) {
        sum += pow(y_true[i] - y_pred[i], 2);
    }
    return sum / y_true.size();
}

vector<double> mse_prime(const vector<double>& y_true, const vector<double>& y_pred) {
    vector<double> result;
    for (size_t i = 0; i < y_true.size(); ++i) {
        result.push_back(2 * (y_pred[i] - y_true[i]) / y_true.size());
    }
    return result;
}