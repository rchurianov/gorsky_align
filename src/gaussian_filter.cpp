
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

const double SIGMA = 1.4;
const double RADIUS = 2;
const double PI = atan(1) * 4;

vector<double> gaussian_kernel_2d(double sigma, int radius) {

    // double factor = 90;
    // double sigma_squared = 2.0;
    double factor = 1 / (2 * PI * sigma * sigma);
    // double factor = 1 / (2 * PI * sigma_squared);
    double divisor = 2.0 * sigma * sigma;
    // double divisor = 2.0 * sigma_squared;
    double sum = 0.0;

    // int number_of_elements = (2 * radius + 1) * (2 * radius + 1);
    vector<double> kernel;

    for (int i = 0; i < 2 * radius + 1; i++) { // rows
        double y = static_cast<double>(i - radius);
        for (int j = 0; j < 2 * radius + 1; j++) { // columns
            double x = static_cast<double>(j - radius);
            kernel.push_back(factor * exp(-((pow(x, 2.0) + pow(y, 2.0)) / divisor)));
            // kernel[i*(2*radius + 1) + j] = exp(-0.5 * (pow(x/sigma, 2.0) + pow(y/sigma, 2.0))) / (2 * PI * sigma * sigma);
            sum += kernel[i*(2*radius + 1) + j];
        }
    }

    /* Division by Sum Normalisation */
    /*
    for (double& d : kernel) {
        // d = d / sum;
        d *= 1000;
    }
    */
    // cout << "sum : " << sum << endl;


    /* Up Scale Normalization with the Smallest Element to 1 */
    double min = kernel[0];
    if (sum < 1) {
        double scale_up = 1.0 + (1.0 - min) / min;
        for (double& d : kernel ) {
            d *= scale_up;
        }
    }

    return kernel;
}

vector<int> get_integer_kernel_2d(const vector<double>& kernel) {
    vector<int> int_kernel;
    for (double d : kernel) {
        int_kernel.push_back((int)round(d));
    }

    // int sum = 0;
    // for (int i : int_kernel) {
    //     sum += i;
    // }
    // cout << sum << endl;

    return int_kernel;
}


int main() {

    int number_of_elements = (2 * RADIUS + 1) * (2 * RADIUS + 1);
    vector<double> kernel_2d(number_of_elements);
    kernel_2d = gaussian_kernel_2d(SIGMA, RADIUS);

    cout << "2D Gaussian Kernel with Sigma = " << SIGMA << " Radius = " << RADIUS << endl;

    cout << setprecision(6) << fixed << setw(8);
    int shift = (2 * RADIUS + 1);
    for (int i = 0; i < 2 * RADIUS + 1; i++) {
        for (int j = 0; j < 2 * RADIUS + 1; j++) {
            cout << kernel_2d[i * shift + j] << " ";
        }
        cout << endl;
    }

    cout << endl;
    cout << "Integer Kernel:" << endl;
    vector<int> int_kernel_2d;
    int_kernel_2d = get_integer_kernel_2d(kernel_2d);
    for (int i = 0; i < 2 * RADIUS + 1; i++) {
        for (int j = 0; j < 2 * RADIUS + 1; j++) {
            cout << setw(2) << int_kernel_2d[i * shift + j] << " ";
        }
        cout << endl;
    }

    return 0;
}
