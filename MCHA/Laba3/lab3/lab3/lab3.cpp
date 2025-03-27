#include <iostream>
#include <cmath>

using namespace std;

double f(double x) {
    return sin( x * x+1) -0.5 ; // уравнение, для которого ищем корни
}

double proizvodnai(double x) {
    return  2 * x * cos( x * x+1); // производная уравнения
}

int signChanges(double a, double b) {
    int count = 0;
    double fa = f(a);
    double fb;

    for (double x = a + 0.1; x <= b; x += 0.1) {
        fb = f(x);
        if (fa * fb < 0) {
            count++;
        }
        fa = fb;
    }

    return count;
}

void bisectionMethod(double a, double b, double epsilon) {
    double c;
    int iterations = 0;

    if (f(a) * f(b) >= 0) {
        cout << "Условие f(a) * f(b) < 0 не выполнено." << endl;
        return;
    }

    while ((b - a) >= epsilon) {
        c = (a + b) / 2;

        if (f(c) == 0.0) {
            break;
        }
        else if (f(c) * f(a) < 0) {
            b = c;
        }
        else {
            a = c;
        }

        iterations++;
    }

    cout << "Метод половинного деления: Корень = " << c << ", Число итераций = " << iterations << endl;
}

void chordMethod(double a, double b, double epsilon) {
    double x1, x2, x3;
    int iterations = 0;

    if (f(a) * f(b) >= 0) {
        cout << "Условие f(a) * f(b) < 0 не выполнено." << endl;
        return;
    }

    x1 = a;
    x2 = b;

    do {
        x3 = x2 - ((f(x2) * (x2 - x1)) / (f(x2) - f(x1)));

        if (f(x3) == 0.0) {
            break;
        }
        else if (f(x3) * f(x1) < 0) {
            x2 = x3;
        }
        else {
            x1 = x3;
        }

        iterations++;
    } while (fabs(f(x3)) >= epsilon);

    cout << "Метод хорд: Корень = " << x3 << ", Число итераций = " << iterations << endl;
}

void newtonMethod(double x0, double epsilon) {
    double x1, x2;
    int iterations = 0;

    x1 = x0;

    do {
        x2 = x1 - (f(x1) / proizvodnai(x1));

        if (f(x2) == 0.0) {
            break;
        }

        x1 = x2;
        iterations++;
    } while (fabs(f(x2)) >= epsilon);

    cout << "Метод Ньютона: Корень = " << x2 << ", Число итераций = " << iterations << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");
    double a = 0;
    double b = 10;
    double epsilon = 0.0001;
    int numRoots = signChanges(a, b);

    cout << "Число корней на отрезке [-10, 10] = " << numRoots << endl;

    bisectionMethod(a, b, epsilon);
    chordMethod(a, b, epsilon);
    newtonMethod(a, epsilon);

    return 0;
}


/*
double f(double x) {
    return x * x * x + 38.4621 * x * x + 364.594 * x + 914.196; // уравнение, для которого ищем корни
}

double proizvodnai(double x) {
    return 3 * x * x + 76.9242 * x + 364.594; // производная уравнения
}


double f(double x) {
    return x * x * x *x - 2 * x * x * x - 7 * x * x + 8 * x +1; // уравнение, для которого ищем корни
}

double proizvodnai(double x) {
    return 4 * x * x * x - 6 * x * x - 14 * x; // производная уравнения
}


double f(double x) {
    return 10 * x - 4; // уравнение, для которого ищем корни
}

double proizvodnai(double x) {
    return  10; // производная уравнения
}

double f(double x) {
    return x * x * x + 8 * x + 10; // уравнение, для которого ищем корни
}

double proizvodnai(double x) {
    return  3 * x * x + 8; // производная уравнения
}
*/