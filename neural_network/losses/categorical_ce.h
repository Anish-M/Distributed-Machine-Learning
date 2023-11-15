#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using std::vector;


double categorical_ce(vector<double> y_true, vector<double> y_pred);

vector<double> categorical_ce_prime(vector<double> y_true, vector<double> y_pred);