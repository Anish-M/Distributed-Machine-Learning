#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

double max_element(const vector<double>& x) {
    double max_x = x[0];
    for (double value : x) {
        if (value > max_x) {
            max_x = value;
        }
    }
    return max_x;
}
// Activation function and its derivative
vector<double> tanh_activation(const vector<double>& x) {
    vector<double> result;
    for (double value : x) {
        result.push_back(tanh(value));
    }
    return result;
}

vector<double> tanh_prime(const vector<double>& x) {
    vector<double> result;
    for (double value : x) {
        result.push_back(1 - tanh(value) * tanh(value));
    }
    return result;
}

vector<double> softmax_activation(const vector<double>& x) {
    vector<double> result;
    double max_x = *max_element(x.begin(), x.end());
    double sum_exp = 0.0;

    for (double value : x) {
        double exp_value = exp(value - max_x);
        result.push_back(exp_value);
        sum_exp += exp_value;
    }

    for (double& value : result) {
        value /= sum_exp;
    }

    return result;
}

vector<double> softmax_prime(const vector<double>& x) {
    vector<double> s = softmax_activation(x);
    vector<double> result;

    for (size_t i = 0; i < s.size(); ++i) {
        result.push_back(s[i] * (1 - s[i]));
    }

    return result;
}

vector<double> sigmoid(const vector<double>& z) {
    vector<double> result;
    for (double value : z) {
        result.push_back(1.0 / (1.0 + exp(-value)));
    }
    return result;
}

vector<double> sigmoid_prime(const vector<double>& z) {
    vector<double> sigmoid_z = sigmoid(z);
    vector<double> result;
    for (size_t i = 0; i < z.size(); ++i) {
        result.push_back(sigmoid_z[i] * (1.0 - sigmoid_z[i]));
    }
    return result;
}

vector<double> relu(const vector<double>& x) {
    vector<double> result;
    for (double value : x) {
        result.push_back(max(0.0, value));
    }
    return result;
}

vector<double> relu_prime(const vector<double>& x) {
    vector<double> result;
    for (double value : x) {
        result.push_back((value <= 0) ? 0.0 : 1.0);
    }
    return result;
}