#include "activation_layer.cpp"
#include "fc_layer.cpp"
#include "losses.cpp"
#include "activations.cpp"
#include "network.cpp"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using  namespace std;
int main() {
    // Training set
    vector<vector<double>> x_train = {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {9, 9}, {9, 8}, {8, 9}, {8, 8}};
    vector<vector<double>> y_train = {{0}, {0}, {0}, {0}, {1}, {1}, {1}, {1}};
    // Create the neural network
    Network net;

    // Add layers and activations
    Layer* fc1 = new FCLayer(2, 3);
    Layer* act1 = new ActivationLayer(tanh_activation, tanh_prime);
    Layer* fc2 = new FCLayer(3, 1);
    Layer* act2 = new ActivationLayer(tanh_activation, tanh_prime);

    net.add(fc1);
    net.add(act1);
    net.add(fc2);
    net.add(act2);

    net.use(mse, mse_prime);

    // // // Train the network
    net.fit(x_train, y_train, 50, 0.1);

    // // // Test the network
    for (size_t i = 0; i < x_train.size(); ++i) {
        vector<double> predicted = net.predict(x_train[i]);
        cout << "Input: (" << x_train[i][0] << ", " << x_train[i][1] << ") Output: " << predicted[0] << endl;
    }


    // test fc2 layer with sample input, output and print the weights
    // FCLayer* fc2 = new FCLayer(2, 3);
    // vector<double> input = {1, 1};
    // vector<double> output = fc2->forwardPropagation(input);
    // // loop through input nad print 
    // cout << "Input: " << endl;
    // for (size_t i = 0; i < input.size(); ++i) {
    //     cout << input[i] << " ";
    // }
    // cout << endl;
    // cout << "Output: " << endl;
    // for (size_t i = 0; i < output.size(); ++i) {
    //     cout << output[i] << " ";
    // }
    // cout << endl;
    // cout << "Weights: " << endl;
    // for (size_t i = 0; i < fc2->weights.size(); ++i) {
    //     for (size_t j = 0; j < fc2->weights[0].size(); ++j) {
    //         cout << fc2->weights[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // test activation layer with sample input, output and print the weights
    // ActivationLayer* act1 = new ActivationLayer(tanh_activation, tanh_prime);
    // vector<double> input = {1, 1};
    // vector<double> output = act1->forwardPropagation(input);
    // // loop through input nad print
    // cout << "Input: " << endl;
    // for (size_t i = 0; i < input.size(); ++i) {
    //     cout << input[i] << " ";
    // }
    // cout << endl;
    // cout << "Output: " << endl;
    // for (size_t i = 0; i < output.size(); ++i) {
    //     cout << output[i] << " ";
    // }
    // cout << endl;

    // test mse and mse prime

}