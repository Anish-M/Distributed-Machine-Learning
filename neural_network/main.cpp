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

int runGenerated() {
    vector<vector<double>> x_train;
    vector<vector<double>> y_train;

    // Read x_train and y_train from file data/generated8000_2_16000.txt
    // n_samples/n_features/n_classes
    // // freopen("../data/generated8000_2_16000.txt", "r", stdin);
    int n_samples, n_features, n_classes;
    n_samples = 10;
    n_features = 5;
    n_classes = 1;

    // Read x_train the first line of the file is 'X' folllowed by n_samples lines of n_features
    string s;
    
    // the first line is 'X'
    // open the file ../data/generated8000_2_16000.txt with cin
    freopen("../data/generated_10_2_5.txt", "r", stdin);
    // read the first line of the file
    cin >> s;
    cout << s << endl;
    // the next n_samples lines are the x_train
    for (int i = 0; i < n_samples; i++) {
        vector<double> x;
        for (int j = 0; j < n_features; j++) {
            double temp;
            cin >> temp;
            x.push_back(temp);
        }
        // cout << "completed reading x_train[" << i << "]" << endl;
        x_train.push_back(x);
    }

    // Read y_train the next line of the file is 'Y' folllowed by n_samples lines of n_classes
    // the next line is 'Y'
    cin >> s;
    cout << s << endl;
    // the next n_samples lines are the y_train
    for (int i = 0; i < n_samples; i++) {
        vector<double> y;
        for (int j = 0; j < n_classes; j++) {
            double temp;
            cin >> temp;
            y.push_back(temp);
        }
        // cout << "completed reading y_train[" << i << "]" << endl;
        y_train.push_back(y);
    }

    // // print the x_train and y_train size
    // cout << "x_train size: " << x_train.size() << endl;
    // cout << "y_train size: " << y_train.size() << endl;
    // // print out all of x_train
    // cout << "x_train: " << endl;
    // for (int i = 0; i < x_train.size(); i++) {
    //     for (int j = 0; j < x_train[i].size(); j++) {
    //         cout << x_train[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // // print out all of y_train
    // cout << "y_train: " << endl;
    // for (int i = 0; i < y_train.size(); i++) {
    //     for (int j = 0; j < y_train[i].size(); j++) {
    //         cout << y_train[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    Network network;

    network.add(new FCLayer(n_features, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime));
    network.add(new FCLayer(5, 8));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(relu, relu_prime)); // Add activation layer with sigmoid
    network.add(new FCLayer(8, 4));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(sigmoid, sigmoid_prime)); // Add activation layer with sigmoid
    network.add(new FCLayer(4, 1));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh_1, tanh_prime)); // Add activation layer with sigmoid


    network.use(mse, mse_prime);     // Use mean squared error loss
    network.fit(x_train, y_train, 50, 0.01);
    return 0;
}

int runExample() {
    // Example usage
    Network network;
    network.add(new FCLayer(3, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(tanh, tanh_prime));

    network.add(new FCLayer(5, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(relu, relu_prime));

    // network.add(new FCLayer(4, 5));  // Add fully connected layer with 3 inputs and 4 outputs
    // network.add(new ActivationLayer(tanh, tanh_prime));
    
    network.add(new FCLayer(5, 1));  // Add fully connected layer with 3 inputs and 4 outputs
    network.add(new ActivationLayer(sigmoid, sigmoid_prime)); // Add activation layer with sigmoid
    network.use(mse, mse_prime);     // Use mean squared error loss

    // Train and predict
    vector<vector<double>> x_train = {{0, 0, 3}, {0, 1, 2}, {1, 0, 1}, {1, 1, 0}, {9, 9, 8}, {9, 8, 7}, {8, 9, 6}, {8, 8, 7}};
    vector<vector<double>> y_train = {{0}, {0}, {0}, {0}, {1}, {1}, {1}, {1}};
    network.fit(x_train, y_train, 50, 0.01);
    vector<double> prediction = network.predict({9, 9, 9});
    for (double val : prediction) cout << val << " ";
    cout << endl;

    // cout << "WORKS BBY" << endl;
    return 0;
}

int main() {
    runGenerated();

    return 0;
}
