#include <iostream>
#include <vector>
#include <cmath>

const double EPSILON = 0.0001;
// Функция для нахождения определителя матрицы
double determinant(std::vector<std::vector<double>>& matrix) {
    double n = matrix.size();
    int det = 0;

    if (n == 1) {
        return matrix[0][0];
    }
    else if (n == 2) {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }
    else {
        for (int i = 0; i < n; ++i) {
            std::vector<std::vector<double>> submatrix(n - 1, std::vector<double>(n - 1));
            for (int j = 1; j < n; ++j) {
                for (int k = 0; k < n; ++k) {
                    if (k < i) {
                        submatrix[j - 1][k] = matrix[j][k];
                    }
                    else if (k > i) {
                        submatrix[j - 1][k - 1] = matrix[j][k];
                    }
                }
            }
            det += matrix[0][i] * determinant(submatrix) * (i % 2 == 0 ? 1 : -1);
        }
    }
    return det;
}
// Метод Гаусса без модификаций
std::vector<double> gaussElimination(const std::vector<std::vector<double>>& A, const std::vector<double>& b) {
    int n = A.size();
    std::vector<std::vector<double>> augmentedMatrix(n, std::vector<double>(n + 1));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmentedMatrix[i][j] = A[i][j];
        }
        augmentedMatrix[i][n] = b[i];
    }

    for (int i = 0; i < n; i++) {
        // Поиск максимального элемента в столбце
        int maxRow = i;
        for (int j = i + 1; j < n; j++) {
            if (std::abs(augmentedMatrix[j][i]) > std::abs(augmentedMatrix[maxRow][i])) {
                maxRow = j;
            }
        }

        // Перестановка строк
        std::swap(augmentedMatrix[i], augmentedMatrix[maxRow]);

        // Приведение к треугольному виду
        for (int j = i + 1; j < n; j++) {
            double factor = augmentedMatrix[j][i] / augmentedMatrix[i][i];
            for (int k = i; k <= n; k++) {
                augmentedMatrix[j][k] -= factor * augmentedMatrix[i][k];
            }
        }
    }

    // Обратный ход
    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = augmentedMatrix[i][n];
        for (int j = i + 1; j < n; j++) {
            x[i] -= augmentedMatrix[i][j] * x[j];
        }
        x[i] /= augmentedMatrix[i][i];
    }

    return x;
}

// Метод Гаусса с выбором главного элемента по столбцу
std::vector<double> gaussPartialPivot(const std::vector<std::vector<double>>& A, const std::vector<double>& b) {
    int n = A.size();
    std::vector<std::vector<double>> augmentedMatrix(n, std::vector<double>(n + 1));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmentedMatrix[i][j] = A[i][j];
        }
        augmentedMatrix[i][n] = b[i];
    }

    for (int i = 0; i < n; i++) {
        // Поиск максимального элемента в столбце
        int maxRow = i;
        for (int j = i + 1; j < n; j++) {
            if (std::abs(augmentedMatrix[j][i]) > std::abs(augmentedMatrix[maxRow][i])) {
                maxRow = j;
            }
        }

        // Перестановка строк
        std::swap(augmentedMatrix[i], augmentedMatrix[maxRow]);

        // Приведение к треугольному виду
        for (int j = i + 1; j < n; j++) {
            double factor = augmentedMatrix[j][i] / augmentedMatrix[i][i];
            for (int k = i; k <= n; k++) {
                augmentedMatrix[j][k] -= factor * augmentedMatrix[i][k];
            }
        }
    }

    // Обратный ход
    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = augmentedMatrix[i][n];
        for (int j = i + 1; j < n; j++) {
            x[i] -= augmentedMatrix[i][j] * x[j];
        }
        x[i] /= augmentedMatrix[i][i];
    }

    return x;
}

// Метод Гаусса с выбором главного элемента по всей матрице
std::vector<double> gaussCompletePivot(const std::vector<std::vector<double>>& A, const std::vector<double>& b) {
    int n = A.size();
    std::vector<std::vector<double>> augmentedMatrix(n, std::vector<double>(n + 1));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            augmentedMatrix[i][j] = A[i][j];
        }
        augmentedMatrix[i][n] = b[i];
    }

    for (int i = 0; i < n; i++) {
        // Поиск максимального элемента во всей матрице
        int maxRow = i;
        int maxCol = i;
        for (int j = i; j < n; j++) {
            for (int k = i; k < n; k++) {
                if (std::abs(augmentedMatrix[j][k]) > std::abs(augmentedMatrix[maxRow][maxCol])) {
                    maxRow = j;
                    maxCol = k;
                }
            }
        }

        // Перестановка строк
        std::swap(augmentedMatrix[i], augmentedMatrix[maxRow]);

        // Перестановка столбцов
        for (int j = 0; j < n; j++) {
            std::swap(augmentedMatrix[j][i], augmentedMatrix[j][maxCol]);
        }

        // Приведение к треугольному виду
        for (int j = i + 1; j < n; j++) {
            double factor = augmentedMatrix[j][i] / augmentedMatrix[i][i];
            for (int k = i; k <= n; k++) {
                augmentedMatrix[j][k] -= factor * augmentedMatrix[i][k];
            }
        }
    }

    // Обратный ход
    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = augmentedMatrix[i][n];
        for (int j = i + 1; j < n; j++) {
            x[i] -= augmentedMatrix[i][j] * x[j];
        }
        x[i] /= augmentedMatrix[i][i];
    }

    return x;
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::vector<std::vector<double>> A =
    { {4,  2,  -1},
    {5, 3, -2},
    {3,  2, -3}};
    
    std::vector<double> b = { 1,  2, 0 };
    int det = determinant(A);
    if (det == 0) {
        std::cout << "Нет решений, т.к. определитель матрицы равен 0 " << std::endl;
    }
    else {
        std::cout << "Определитель матрицы: " << det << std::endl;
        std::vector<double> x1 = gaussElimination(A, b);
        std::vector<double> x2 = gaussPartialPivot(A, b);
        std::vector<double> x3 = gaussCompletePivot(A, b);

        std::cout << "Метод Гаусса без модификаций:" << std::endl;
        for (int i = 0; i < x1.size(); i++) {
            std::cout << "x[" << i << "] = " << x1[i] << std::endl;
        }

        std::cout << "Метод Гаусса с выбором главного элемента по столбцу:" << std::endl;
        for (int i = 0; i < x2.size(); i++) {
            std::cout << "x[" << i << "] = " << x2[i] << std::endl;
        }

        std::cout << "Метод Гаусса с выбором главного элемента по всей матрице:" << std::endl;
        for (int i = 0; i < x3.size(); i++) {
            std::cout << "x[" << i << "] = " << x3[i] << std::endl;
        }
    }
    return 0;
}
/* A = { {3.73, 0.81, 2.07, 0.92, -0.53},
                                            {-0.53, 3.73, 0.81, 2.07, 0.92},
                                            {2.32, -0.53, 3.73, 0.81, 2.07},
                                            {0.67, 2.32, -0.53, 3.73, 0.81},
                                            {0.81, 0.67, 2.32, -0.53, 3.73} };
    std::vector<double> b = { 4.2, 4.2, 4.2, 4.2, 4.2 };




    A =
    { {4,  0,  3,  0, -0},
    {-0, 4, 0, 3, 0},
        {3, 0, 4, 0, 3},
    {0,  3, 5, 4, 0},
    {0, 0, 3, 3, 4} };

    std::vector<double> b = { 0,  0, 1, 6, 0 };
    */