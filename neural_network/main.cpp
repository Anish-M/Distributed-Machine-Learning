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
    // Training set read x_train and y_train
    vector<vector<double>> x_train;
    vector<vector<double>> y_train;

    // Read x_train and y_train from file data/generated8000_2_16000.txt
    // n_samples/n_features/n_classes
    // // freopen("../data/generated8000_2_16000.txt", "r", stdin);
    int n_samples, n_features, n_classes;
    n_samples = 10;
    n_features = 5;
    n_classes = 2;

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

    // print the x_train and y_train size
    cout << "x_train size: " << x_train.size() << endl;
    cout << "y_train size: " << y_train.size() << endl;
    // print the first x_train and y_train
    cout << "x_train[0]: ";
    for (int i = 0; i < x_train[0].size(); i++) {
        cout << x_train[0][i] << " ";
    }
    cout << endl;
    cout << "y_train[0]: ";
    for (int i = 0; i < y_train[0].size(); i++) {
        cout << y_train[0][i] << " ";
    }
    cout << endl;

    

    Network net;

    // net = Network()
    // net.add(FCLayer(10000, 7500))
    // net.add(ActivationLayer(tanh, tanh_prime))
    // net.add(FCLayer(7500, 5000))
    // net.add(ActivationLayer(tanh, tanh_prime))
    // net.add(FCLayer(5000, 2500))
    // net.add(ActivationLayer(tanh, tanh_prime))
    // net.add(FCLayer(2500, 1000))
    // net.add(ActivationLayer(tanh, tanh_prime))
    // net.add(FCLayer(1000, 500))
    // net.add(ActivationLayer(tanh, tanh_prime))
    // net.add(FCLayer(500, 250))
    // net.add(ActivationLayer(tanh, tanh_prime))
    // net.add(FCLayer(250, 2))
    // net.add(ActivationLayer(softmax, softmax_prime))

    Layer* fc1 = new FCLayer(1000, 7500);
    Layer* act1 = new ActivationLayer(tanh_activation, tanh_prime);
    Layer* fc2 = new FCLayer(7500, 5000);
    Layer* act2 = new ActivationLayer(tanh_activation, tanh_prime);
    Layer* fc3 = new FCLayer(5000, 2500);
    Layer* act3 = new ActivationLayer(tanh_activation, tanh_prime);
    Layer* fc4 = new FCLayer(2500, 1000);
    Layer* act4 = new ActivationLayer(tanh_activation, tanh_prime);
    Layer* fc5 = new FCLayer(1000, 500);
    Layer* act5 = new ActivationLayer(tanh_activation, tanh_prime);
    Layer* fc6 = new FCLayer(500, 250);
    Layer* act6 = new ActivationLayer(tanh_activation, tanh_prime);
    Layer* fc7 = new FCLayer(250, 2);
    Layer* act7 = new ActivationLayer(softmax_activation, softmax_prime);

    net.add(fc1);
    net.add(act1);
    net.add(fc2);
    net.add(act2);
    net.add(fc3);
    net.add(act3);
    net.add(fc4);
    net.add(act4);
    net.add(fc5);
    net.add(act5);
    net.add(fc6);
    net.add(act6);
    net.add(fc7);
    net.add(act7);

    net.use(categorical_cross_entropy, categorical_cross_entropy_prime);

    // Train
    net.fit(x_train, y_train, 1, 0.1);



}