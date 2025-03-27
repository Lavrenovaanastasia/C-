#include <iostream>
#include <vector>
#include <cmath>

// Метод простых итераций
std::vector<double> simpleIterationMethod(const std::vector<std::vector<double>>& A,
    const std::vector<double>& b, double epsilon) {
    int n = A.size();
    std::vector<double> x(n, 0.0);
    std::vector<double> prevX(n, 0.0);
    double normDiff = epsilon + 1.0;

    while (normDiff > epsilon) {
        prevX = x;
        for (int i = 0; i < n; ++i) {
            double sum = 0.0;
            for (int j = 0; j < n; ++j) {
                if (j != i) {
                    sum += A[i][j] * prevX[j];
                }
            }
            x[i] = (b[i] - sum) / A[i][i];
        }

        normDiff = 0.0;
        for (int i = 0; i < n; ++i) {
            normDiff += std::abs(x[i] - prevX[i]);
        }
    }
    return x;
}

// Метод Зейделя
std::vector<double> gaussSeidelMethod(const std::vector<std::vector<double>>& A,
    const std::vector<double>& b, double epsilon) {
    int n = A.size();
    std::vector<double> x(n, 0.0);
    std::vector<double> prevX(n, 0.0);
    double normDiff = epsilon + 1.0;

    while (normDiff > epsilon) {
        prevX = x;
        for (int i = 0; i < n; ++i) {
            double sum1 = 0.0;
            double sum2 = 0.0;
            for (int j = 0; j < i; ++j) {
                sum1 += A[i][j] * x[j];
            }
            for (int j = i + 1; j < n; ++j) {
                sum2 += A[i][j] * prevX[j];
            }
            x[i] = (b[i] - sum1 - sum2) / A[i][i];
        }

        normDiff = 0.0;
        for (int i = 0; i < n; ++i) {
            normDiff += std::abs(x[i] - prevX[i]);
        }
    }
    return x;
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::vector<std::vector<double>>  A =  { {2}, };

    std::vector<double> b = { 2};
    double epsilon = 0.0001;

    std::vector<double> resultSimpleIteration = simpleIterationMethod(A, b, epsilon);
    std::vector<double> resultGaussSeidel = gaussSeidelMethod(A, b, epsilon);

    std::cout << "Результат метода простых итераций: ";
    for (const auto& x : resultSimpleIteration) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "Результат метода Зейделя: ";
    for (const auto& x : resultGaussSeidel) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    return 0;
}


/* std::vector<std::vector<double>> A = { {4.0, -1.0, 0.0},
                                          {-1.0, 4.0, -1.0},
                                          {0.0, -1.0, 4.0} };
    std::vector<double> b = { 1.2, 2.0, 3.0 };
    double epsilon = 0.0001;
    
    
    
    
    setlocale(LC_ALL, "Russian");
    std::vector<std::vector<double>> A = { {3.73, 0.81, 2.07, 0.92, -0.53},
                                          {-0.53, 3.73, 0.81, 2.07, 0.92},
                                          {2.32, -0.53, 3.73, 0.81, 2.07},
                                          {0.67, 2.32, -0.53, 3.73, 0.82},
                                          {0.81, 0.67, 2.32, -0.53, 3.73}, };
    std::vector<double> b = { 1.2, 2.2, 4.0, 0.0, -1.2 };
    double epsilon = 0.0001;
    
    
    
    std::vector<std::vector<double>> A =
    { {4,  2,  -1},
    {5, 3, -2},
    {3,  2, -3}};
    
    std::vector<double> b = { 1,  2, 0 };
    
    
    
    
    A =  { {0.1, 7},
                                             {7, -0.1} };

    std::vector<double> b = { 2, 3};*/