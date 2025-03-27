#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

vector<int> X = { 1, 3, 8, 9, 15 };
vector<int> Y = { 7, 11, 18, 19, 21 };
vector<int> mergedArrayManual(X.size() + Y.size());
int i = 0, j = 0, k = 0;
int iteration = 1;

// Output current merged array with iteration number
void Timer(chrono::steady_clock::time_point startTime)
{
    cout << "Итерация " << iteration << ". Текущий объединенный массив: ";
    for (int m = 0; m < k; m++)
        cout << mergedArrayManual[m] << " ";
    cout << endl;
    // Подсчет времени на каждую итерацию
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsedTime = chrono::duration_cast<chrono::duration<double>>(endTime - startTime);
    cout << "Время на итерацию: " << elapsedTime.count() << " секунд" << endl;
    iteration++; // Увеличение номера итерации
}


//-----------------------------------------------------БИНАРНЫЙ ПОИСК--------------------------------------------
double findMedianSortedArrays(vector<int>& X, vector<int>& Y) {
    int n = X.size();
    //n-длинна массива
    int l = 0, r = n;//l (левая граница диапазона поиска) и r (правая граница диапазона поиска)
    //------------------------------СЛИЯНИЕ МАССИВОВ ПРИ ПОМОЩИ ВСТРОЕННОЙ ФУНКЦИИ mergedArray-------------------
    vector<int> mergedArray(n + n);//объединённый и отсортированный массив

    while (l <= r) {//пока левая граница l не превысит правую границу r
        int midX = (l + r) / 2;//определение середины 
        int midY = n - midX;//правая пловина
        //определение максимального значения слева от разделителя
        int maxX = (midX == 0) ? INT_MIN : X[midX - 1];
        int minX = (midX == n) ? INT_MAX : X[midX];
        //определение максимального значения справа от разделителя
        int maxY = (midY == 0) ? INT_MIN : Y[midY - 1];
        int minY = (midY == n) ? INT_MAX : Y[midY];

        if (maxX <= minY && maxY <= minX) {
            if ((n + n) % 2 == 0) {
                //для нечётного массива
                double median = (max(maxX, maxY) + min(minX, minY)) / 2.0;
                cout << "Найдены потенциальные средние кандидаты: " << max(maxX, maxY) << " and " << min(minX, minY) << endl;
                return median;
            }
            //для чтного массива
            else {
                double median = min(minX, minY);
                cout << "Найден потенциальный средний кандидат: " << median << endl;
                return median;
            }
        }
        else if (maxX > minY) {
            r = midX - 1;
            //cout << "Настройка диапазона поиска: l = " << l << ", r = " << r << endl;
        }
        else {
            l = midX + 1;
            //cout << "Настройка диапазона поиска: l = " << l << ", r = " << r << endl;
        }
    }

    return -1;
}
//-----------------------------------------------MAIN----------------------------------------------
int main() {
    setlocale(LC_ALL, "Russian");

    double median = findMedianSortedArrays(X, Y);
    cout << "Медиана объединенных массивов X и Y: " << median << endl;

    auto startTimeMerge = chrono::high_resolution_clock::now(); // Начало временных измерений для merge
    vector<int> mergedArray;
    merge(X.begin(), X.end(), Y.begin(), Y.end(), back_inserter(mergedArray));
    auto endTimeMerge = chrono::high_resolution_clock::now(); // Конец временных измерений для merge

    cout << "Объединенный массив после слияния с помощью функции merge: ";
    for (int num : mergedArray)
        cout << num << " ";
    cout << endl;

    // Подсчет времени на слияние массивов при помощи функции merge
    chrono::duration<double> elapsedTimeMerge = chrono::duration_cast<chrono::duration<double>>(endTimeMerge - startTimeMerge);
    cout << "Время на слияние массивов с помощью функции merge: " << elapsedTimeMerge.count() << " секунд" << endl;

    // Manual merging
    cout << "Объединение массивов X и Y вручную:" << endl;

    // Инициализация переменной для подсчёта времени
    auto startTime = chrono::high_resolution_clock::now();
    while (i < X.size() && j < Y.size()) {
        if (X[i] < Y[j]) {
            mergedArrayManual[k] = X[i];
            i++;
        }
        else {
            mergedArrayManual[k] = Y[j];
            j++;
        }
        k++;
        Timer(startTime);
    }

    // записываем остаток
    while (i + j < X.size() + Y.size())
    {
        if (i < X.size()) {
            mergedArrayManual[k] = X[i];
            i++;
        }
        else if (j < Y.size()) {
            mergedArrayManual[k] = Y[j];
            j++;
        }
        k++;
        Timer(startTime);
    }

    // Окончательный вывод объединенного массива
    cout << "Окончательный объединенный массив после ручного объединения X и Y: ";
    for (int num : mergedArrayManual) {
        cout << num << " ";
    }
    cout << endl;

    return 0;
}