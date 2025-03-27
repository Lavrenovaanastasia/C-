#include <iostream>
using namespace std;

const int MAX_SIZE = 5; // Размер матрицы

// Функция для умножения матрицы на число
void multiplyMatrixByNumber(double matrix[MAX_SIZE][MAX_SIZE], int number) {
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            matrix[i][j] *= number;
        }
    }
}

// Функция для сложения двух матриц
void addMatrices(double matrix1[MAX_SIZE][MAX_SIZE], double matrix2[MAX_SIZE][MAX_SIZE]) {
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            matrix1[i][j] += matrix2[i][j];
        }
    }
}

// Функция для вывода матрицы
void printMatrix(double matrix[MAX_SIZE][MAX_SIZE]) {
    cout << "Матрица:\n";
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

int main() {

    setlocale(LC_ALL, "Russian");
    double matrix1[MAX_SIZE][MAX_SIZE] = {{0.01, 0, -0.02, 0, 0},
                                        {0.01, 0.01, -0.02, 0, 0},
                                        {0, 0.01, 0.01, 0, -0.02},
                                        {0, 0, 0.01, 0.01, 0},
                                        {0, 0, 0, 0.01, 0.01} };

   double matrix2[MAX_SIZE][MAX_SIZE] = {{1.33, 0.21, 0.17, 0.12, -0.13},
                                        {-0.13, 1.33, 0.11, 0.17, 0.12},
                                        {0.12, -0.13, 1.33, 0.11, 0.17},
                                        {0.17, 0.12, -0.13, 1.33, 0.11},
                                        {0.11, 0.67, 0.12, -0.13, 1.33} };

    int number = 7; // Число для умножения матрицы

    multiplyMatrixByNumber(matrix1, number);
    addMatrices(matrix1, matrix2);
    printMatrix(matrix1);

    return 0;
}