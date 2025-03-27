#include <iostream>
#include <cmath>

using namespace std;

double f1(double x, double y) {
    return 2*x-y-8;
}

double f2(double x, double y) {
    return 3*x+2*y-5;
}

double df1_dx(double x, double y) {
    return  2;
}

double df1_dy(double x, double y) {
    return -1;
}

double df2_dx(double x, double y) {
    return 3;
}

double df2_dy(double x, double y) {
    return 2;
}

void simpleIterationMethod(double x0, double y0, double epsilon) {
    double x = x0, y = y0;
    double x_prev, y_prev;
    int iterations = 0;

    do {
        x_prev = x;
        y_prev = y;

        double det = df1_dx(x_prev, y_prev) * df2_dy(x_prev, y_prev) - df1_dy(x_prev, y_prev) * df2_dx(x_prev, y_prev);

        x = x_prev - (f1(x_prev, y_prev) * df2_dy(x_prev, y_prev) - f2(x_prev, y_prev) * df1_dy(x_prev, y_prev)) / det;
        y = y_prev - (f2(x_prev, y_prev) * df1_dx(x_prev, y_prev) - f1(x_prev, y_prev) * df2_dx(x_prev, y_prev)) / det;

        iterations++;
    } while (abs(x - x_prev) > epsilon || abs(y - y_prev) > epsilon);

    cout << "Simple Iteration Method:" << endl;
    cout << "x = " << x << ", y = " << y << endl;
    cout << "Iterations: " << iterations << endl;
}

void newtonMethod(double x0, double y0, double epsilon) {
    double x = x0, y = y0;
    double x_prev, y_prev;
    int iterations = 0;

    do {
        x_prev = x;
        y_prev = y;

        double det = df1_dx(x_prev, y_prev) * df2_dy(x_prev, y_prev) - df1_dy(x_prev, y_prev) * df2_dx(x_prev, y_prev);

        x = x_prev - (f1(x_prev, y_prev) * df2_dy(x_prev, y_prev) - f2(x_prev, y_prev) * df1_dy(x_prev, y_prev)) / det;
        y = y_prev - (f2(x_prev, y_prev) * df1_dx(x_prev, y_prev) - f1(x_prev, y_prev) * df2_dx(x_prev, y_prev)) / det;

        iterations++;
    } while (abs(x - x_prev) > epsilon || abs(y - y_prev) > epsilon);

    cout << "Newton's Method:" << endl;
    cout << "x = " << x << ", y = " << y << endl;
    cout << "Iterations: " << iterations << endl;
}

int main() {
    double x0, y0, epsilon;

    cout << "Enter initial values (x0 y0): ";
    cin >> x0 >> y0;

    cout << "Enter the desired precision: ";
    cin >> epsilon;

    simpleIterationMethod(x0, y0, epsilon);
    newtonMethod(x0, y0, epsilon);

    return 0;
}










/* 
Само задание
//Функция 1
    f = [](double x)
    {
        double y = sqrt(2-x*x)/4;
        return y;
    };
    function_point_cloud(f, Data_, x_min, x_max, step);
    funcs.push_back(Data_);


    //Функция 2
    Data_.clear();
    fp = [](double x)
    {
        double y = (x-0,3)/tan(x);
        return  y;
    };
    function_point_cloud(fp, Data_, x_min, x_max, step);
    funcs.push_back(Data_);
    
    
    
double f1(double x, double y) {
    return tan(x * y) + 0.3 - x;
}

double f2(double x, double y) {
    return 0.5 * pow(x, 2) + 2 * pow(y, 2) - 1;
}

double df1_dx(double x, double y) {
    return -tan(x * y) + 1;
}

double df1_dy(double x, double y) {
    return -x * pow(1 / cos(x * y), 2);
}

double df2_dx(double x, double y) {
    return x;
}

double df2_dy(double x, double y) {
    return 4 * y;
}
    
    
    
    Пример1

     //Функция 1
    f = [](double x)
    {
        double y = (7-x)/5;
        return y;
    };
    function_point_cloud(f, Data_, x_min, x_max, step);
    funcs.push_back(Data_);


    //Функция 2
    Data_.clear();
    fp = [](double x)
    {
        double y = (3*x+4)/2;
        return  y;
    };
    function_point_cloud(fp, Data_, x_min, x_max, step);
    funcs.push_back(Data_);
    
    
double f1(double x, double y) {
    return x+5*y-7;
}

double f2(double x, double y) {
    return 3*x-2*y-4;
}

double df1_dx(double x, double y) {
    return  1;
}

double df1_dy(double x, double y) {
    return 5;
}

double df2_dx(double x, double y) {
    return 3;
}

double df2_dy(double x, double y) {
    return -2;
}
    


    Пример 2
     //Функция 1
    f = [](double x)
    {
        double y = 2*x-8;
        return y;
    };
    function_point_cloud(f, Data_, x_min, x_max, step);
    funcs.push_back(Data_);


    //Функция 2
    Data_.clear();
    fp = [](double x)
    {
        double y = (5 - 3*x)/2;
        return  y;
    };
    function_point_cloud(fp, Data_, x_min, x_max, step);
    funcs.push_back(Data_);
    


    double f1(double x, double y) {
    return 2*x-y-8;
}

double f2(double x, double y) {
    return 3*x+2*y-5;
}

double df1_dx(double x, double y) {
    return  2;
}

double df1_dy(double x, double y) {
    return -1;
}

double df2_dx(double x, double y) {
    return 3;
}

double df2_dy(double x, double y) {
    return 2;
}
    
    */