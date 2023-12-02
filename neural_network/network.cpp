#include <iostream>
#include <vector>
#include <ctime>
#include "layer.h"
using namespace std;

class Network {
private:
    vector<Layer*> layers;
    double (*loss)(const vector<double>&, const vector<double>&);
    vector<double> (*lossPrime)(const vector<double>&, const vector<double>&);

    public:
    Network() : loss(nullptr), lossPrime(nullptr) {}

    void add(Layer* layer) {
        layers.push_back(layer);
    }
    
    void use(double (*lossFunc)(const vector<double>&, const vector<double>&), vector<double> (*lossPrimeFunc)(const vector<double>&, const vector<double>&)) {
        loss = lossFunc;
        lossPrime = lossPrimeFunc;
    }

    vector<double> predict(const vector<double>& input_data) {
        vector<double> result = input_data;

        for (Layer* layer : layers) {
            result = layer->forwardPropagation(result);
        }

        return result;
    }

    void fit(vector<vector<double>>& x_train, vector<vector<double>>& y_train, int epochs, double learning_rate) {
        // Sample dimension first
        int samples = x_train.size();

        // Training loop
        for (int i = 0; i < epochs; ++i) {
            double err = 0;

            for (int j = 0; j < samples; ++j) {
                // Forward propagation
                vector<double> output = x_train[j];
                for (Layer* layer : layers) {
                    output = layer->forwardPropagation(output);
                }


                // Compute loss (for display purpose only)
                err += loss(y_train[j], output);

                // Backward propagation
                vector<double> error = lossPrime(y_train[j], output);
                for (auto layerIter = layers.rbegin(); layerIter != layers.rend(); ++layerIter) {
                    Layer* layer = *layerIter;
                    error = layer->backwardPropagation(error, learning_rate);
                }

                // loop through and print y_train[j]
                // cout << "y_train[j]: "  <<  y_train[j][0] << endl;
                // cout << "output: " << output[0] << endl;

                // cout << "output: " << output << endl;
                // cout << "error: " << error << endl;
            }

            // Calculate average error on all samples
            err /= samples;
            cout << "epoch " << i + 1 << "/" << epochs << "   error=" << err << endl;
        }
    }
};


