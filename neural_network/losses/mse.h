#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using std::vector;


double mse(vector<double> y_true, vector<double> y_pred);

vector<double> mse_prime(vector<double> y_true, vector<double> y_pred);