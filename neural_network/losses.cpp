#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

double mse(const vector<double>& y_true, const vector<double>& y_pred) {
    double sum = 0.0;
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

double categorical_cross_entropy(const vector<double>& y_true, const vector<double>& y_pred) {
    const double epsilon = 1e-15;
    double sum = 0.0;
    for (size_t i = 0; i < y_true.size(); ++i) {
        double clipped_pred = max(epsilon, min(1.0 - epsilon, y_pred[i]));
        sum += -y_true[i] * log(clipped_pred);
    }
    return sum;
}

vector<double> categorical_cross_entropy_prime(const vector<double>& y_true, const vector<double>& y_pred) {
    const double epsilon = 1e-15;
    vector<double> result;
    for (size_t i = 0; i < y_true.size(); ++i) {
        double clipped_pred = max(epsilon, min(1.0 - epsilon, y_pred[i]));
        result.push_back(-y_true[i] / clipped_pred);
    }
    return result;
}
