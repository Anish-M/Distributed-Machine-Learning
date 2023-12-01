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
    vector<vector<double>> x_train = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> y_train = {{0}, {1}, {1}, {0}};
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

    // // Train the network
    // net.fit(x_train, y_train, 10000, 0.1);

    // // Test the network
    // for (size_t i = 0; i < x_train.size(); ++i) {
    //     vector<double> predicted = net.predict(x_train[i]);
    //     cout << "Input: (" << x_train[i][0] << ", " << x_train[i][1] << ") Output: " << predicted[0] << endl;
    // }

    // test teh activation layer with tanh 
    act1->forwardPropagation(x_train[3]);
    act1->backwardPropagation(y_train[3], 0.1);
    cout << "Input: (" << x_train[0][0] << ", " << x_train[0][1] << ") Output: " << act1->forwardPropagation(x_train[0])[0] << endl;
    cout << "Input: (" << x_train[0][0] << ", " << x_train[0][1] << ") Output: " << act1->backwardPropagation(y_train[0], 0.1)[0] << endl;


    // net.add_activation(tanh, tanh_prime);
    // net.add(FCLayer(3, 1));
    // net.add_activation(tanh, tanh_prime);
}