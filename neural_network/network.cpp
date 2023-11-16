#include <iostream>
#include <vector>
#include <ctime>

using std::vector;

// Forward declaration of Layer class
class Layer;

// Base Class
class Layer {
public:
    vector<double> input;
    vector<double> output;

    // Computes the output Y of a layer for a given input X
    virtual void forwardPropagation() = 0;

    // Computes dE/dX for a given dE/dY (and update parameters if any)
    virtual void backwardPropagation(const vector<double>& outputError, double learningRate) = 0;
};

// Network Class
class Network {
private:
    vector<Layer*> layers;
    double (*loss)(const vector<double>&, const vector<double>&);
    vector<double> (*lossPrime)(const vector<double>&, const vector<double>&);

public:
    // Constructor
    Network() : loss(nullptr), lossPrime(nullptr) {}

    // Destructor
    ~Network() {
        for (Layer* layer : layers) {
            delete layer;
        }
    }

    // Add layer to network
    void add(Layer* layer) {
        layers.push_back(layer);
    }

    // Set loss to use
    void use(double (*lossFunc)(const vector<double>&, const vector<double>&),
             vector<double> (*lossPrimeFunc)(const vector<double>&, const vector<double>&)) {
        loss = lossFunc;
        lossPrime = lossPrimeFunc;
    }

    // Predict output for given input
    vector<vector<double>> predict(const vector<vector<double>>& inputData) {
        vector<vector<double>> result;

        // Run network over all samples
        for (const auto& input : inputData) {
            // Forward propagation
            vector<double> output = input;
            for (Layer* layer : layers) {
                layer->input = output;
                layer->forwardPropagation();
                output = layer->output;
            }
            result.push_back(output);
        }

        return result;
    }

    // Train the network
    void fit(const vector<vector<double>>& xTrain, const vector<vector<double>>& yTrain,
             int epochs, double learningRate) {
        // Sample dimension first
        int samples = xTrain.size();

        // Training loop
        for (int i = 0; i < epochs; ++i) {
            double err = 0;
            clock_t startEpochTime = clock();

            for (int j = 0; j < samples; ++j) {
                clock_t startTime = clock();

                // Forward propagation
                vector<double> output = xTrain[j];
                for (Layer* layer : layers) {
                    layer->input = output;
                    layer->forwardPropagation();
                    output = layer->output;
                }

                // Compute loss (for display purpose only)
                std::cout << "\rEpoch " << i + 1 << "/" << epochs << " Sample " << j + 1 << "/" << samples
                          << "   Output:" << output[0] << " Y_Train[j]: " << yTrain[j][0] << " ---" << std::flush;
                err += loss(yTrain[j], output);

                // Backward propagation
                vector<double> error = lossPrime(yTrain[j], output);
                for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
                    (*it)->backwardPropagation(error, learningRate);
                    error = (*it)->input;  // Update error for the next layer
                }

                std::cout << "\rEpoch " << i + 1 << "/" << epochs << " Sample " << j + 1 << "/" << samples
                          << "   Time: " << static_cast<double>(clock() - startTime) / CLOCKS_PER_SEC << "s" << std::flush;
            }

            // Calculate average error on all samples
            err /= samples;
            double timeForEpoch = static_cast<double>(clock() - startEpochTime) / CLOCKS_PER_SEC;
            std::cout << "\nepoch " << i + 1 << "/" << epochs << "   error=" << err << std::endl;
            std::cout << "\t time for epoch=" << timeForEpoch << "s" << std::endl;
        }
    }
};

// Example usage:
int main() {
    // Create a network
    Network network;

    // Add layers to the network (implementation of Layer class needed)
    // ...

    // Set loss functions for the network (implementation needed)
    // network.use(lossFunction, lossPrimeFunction);

    // Prepare training data (xTrain, yTrain)
    // ...

    // Train the network
    // network.fit(xTrain, yTrain, epochs, learningRate);

    return 0;
}
