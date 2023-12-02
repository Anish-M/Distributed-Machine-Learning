#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <ctime>
#include "Network.hpp"
#include "FCLayer.hpp"
#include "ActivationLayer.hpp"
#include "Activations.hpp"

using namespace std;

int main() {
    // Example usage
    Network network;
    network.add(new FCLayer(3, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(5, 1));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime)); // Add activation layer with sigmoid
    network.use(mse, mse_prime);     // Use mean squared error loss

    // Train and predict
    vector<vector<double>> x_train = {{0, 0, 3}, {0, 1, 2}, {1, 0, 1}, {1, 1, 0}, {9, 9, 8}, {9, 8, 7}, {8, 9, 6}, {8, 8, 7}};
    vector<vector<double>> y_train = {{0}, {0}, {0}, {0}, {1}, {1}, {1}, {1}};
    network.fit(x_train, y_train, 50, 0.01);
    vector<double> prediction = network.predict({9, 9, 9});
    for (double val : prediction) cout << val << " ";
    cout << endl;

    return 0;
}
