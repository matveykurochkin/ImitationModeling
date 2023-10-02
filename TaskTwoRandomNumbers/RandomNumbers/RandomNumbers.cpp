#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <iomanip>

// Наша функция генерации ПСЧ метод: "Линейный конгруэнтный метод"
float* ourRealizationRand(int n) {
    float* r = new float[n];  // Создаем массив для хранения сгенерированных чисел
    int y;  
    static int b = time(NULL);  // Статическая переменная для инициализации начального значения

    for (int i = 0; i < n; i++) {  // Цикл для генерации n чисел
        y = b * 1220703125;  // Генерация промежуточного числа
        if (y < 0) {  // Если число отрицательное, исправляем его
            y += 2 * 1073741824;
        }
        r[i] = y * 0.0000000004656613;  // Масштабирование числа в интервал [0, 1)
        b = y;  // Обновление начального значения для следующей итерации
    }

    return r;  // Возвращаем массив с сгенерированными числами
}


// Генератор случайных чисел из стандартной библиотеки C++ mt19937
float* generateRandomNumbers(int n) {
    std::mt19937 rng(time(NULL)); // Инициализация генератора случайных чисел с использованием текущего времени
    std::uniform_real_distribution<float> distribution(0.0, 1.0); // Задание равномерного распределения на интервале [0.0, 1.0)
    float* r = new float[n]; // Создание динамического массива для хранения сгенерированных чисел

    for (int i = 0; i < n; i++) {
        r[i] = distribution(rng); // Генерация одного случайного числа и сохранение его в массиве
    }

    return r; // Возвращение указателя на массив с сгенерированными числами
}


// Создание и заполнение гистограммы
void createHistogram(const float* numbers, int n, std::vector<int>& histogram, float intervalSize) {
    int numBins = static_cast<int>(1.0 / intervalSize) + 1; // Вычисление количества интервалов в гистограмме, увеличиваем на 1 для учета последнего интервала

    for (int i = 0; i < n; i++) { // Проход по всем сгенерированным числам
        int intervalNumber = static_cast<int>(numbers[i] / intervalSize); // Вычисление номера интервала для текущего числа
        if (intervalNumber >= 0 && intervalNumber < numBins) { // Проверка, что номер интервала находится в допустимых пределах
            histogram[intervalNumber]++; // Увеличение счетчика для соответствующего интервала
        }
    }
}


// Вывод гистограммы в виде графика
void printHistogram(const std::vector<int>& histogram, float intervalSize) {
    int numIntervals = static_cast<int>(1.0 / intervalSize) + 1; // Вычисление количества интервалов в гистограмме, увеличиваем на 1 для учета последнего интервала
    int maxCount = *std::max_element(histogram.begin(), histogram.end()); // Нахождение максимального значения среди счетчиков интервалов

    for (int i = 0; i < numIntervals; i++) { // Проход по всем интервалам в гистограмме
        float intervalStart = i * intervalSize; // Начало текущего интервала
        float intervalEnd = (i + 1) * intervalSize; // Конец текущего интервала
        int count = histogram[i]; // Получение счетчика для текущего интервала
        int barLength = static_cast<int>(static_cast<double>(count) / maxCount * 50); // Вычисление длины столбца для графика

        std::cout << "[" << std::fixed << std::setprecision(2) << intervalStart << ", " << intervalEnd << "]: "; // Вывод интервала с округленными значениями
        for (int j = 0; j < barLength; j++) { // Отображение столбца графика с использованием символа "*"
            std::cout << "*";
        }
        std::cout << " " << count << std::endl; // Вывод счетчика для текущего интервала
    }
}


// Тест X-Square
void testXSquare(const float* numbers, int n, int numIntervals, float intervalSize) {
    std::vector<int> observed(numIntervals, 0); // Создание вектора для наблюдаемых частот в интервалах гистограммы

    // Заполнение observed согласно гистограмме
    for (int i = 0; i < n; i++) {
        int interval = static_cast<int>(numbers[i] / intervalSize); // Вычисление номера интервала для текущего числа
        if (interval >= 0 && interval < numIntervals) {
            observed[interval]++; // Увеличение счетчика наблюдаемых частот для соответствующего интервала
        }
    }

    // Вычисление среднего ожидаемого значения для всех интервалов
    float expected = static_cast<float>(n) / static_cast<float>(numIntervals);

    // Вычисление статистики хи-квадрат
    float xSquare = 0.0;
    for (int i = 0; i < numIntervals; i++) {
        xSquare += pow(observed[i] - expected, 2) / expected; // Вычисление вклада каждого интервала в статистику хи-квадрат
    }

    // Проверка по критическому значению
    float criticalValue = 16.9; // Значение для alpha=0.05 и degreesOfFreedom=numIntervals-1 (то есть 9), взято отсюда: https://math.semestr.ru/group/xixi.php
    if (xSquare < criticalValue) {
        std::cout << "Тест X-Square пройден (X-Square = " << xSquare << ")" << std::endl;
    }
    else {
        std::cout << "Тест X-Square не пройден (X-Square = " << xSquare << ")" << std::endl;
    }
}


int main() {
    setlocale(LC_ALL, "Russian"); // Установка локали для вывода на русском языке
    const int quantityGeneratingNumbers = 10000; // Количество генерируемых чисел
    const float intervalSize = 0.1; // Размер интервала

    // Генерация чисел с помощью вашей функции
    float* myNumbers = ourRealizationRand(quantityGeneratingNumbers);

    // Генерация чисел с помощью библиотечного генератора
    float* libraryNumbers = generateRandomNumbers(quantityGeneratingNumbers);

    // Создание и заполнение гистограмм для обоих методов
    int numBins = 10;
    std::vector<int> myHistogram(numBins, 0); // Гистограмма для наших чисел
    std::vector<int> libraryHistogram(numBins, 0); // Гистограмма для библиотечных чисел

    createHistogram(myNumbers, quantityGeneratingNumbers, myHistogram, intervalSize); // Создание гистограммы для наших чисел
    createHistogram(libraryNumbers, quantityGeneratingNumbers, libraryHistogram, intervalSize); // Создание гистограммы для библиотечных чисел

    std::cout << "Наша функция генерации ПСЧ Линейным конгруэнтным методом:" << std::endl;

    std::cout << "Гистограмма для нашего генератора:" << std::endl;
    printHistogram(myHistogram, intervalSize); // Вывод гистограммы для наших чисел

    std::cout << "Тест X-Square для нашего генератора:" << std::endl;
    testXSquare(myNumbers, quantityGeneratingNumbers, numBins, intervalSize); // Проведение теста X-Square для наших чисел

    std::cout << "**********************************************************************" << std::endl;

    std::cout << "Функция генерации ПСЧ библиотечным генератором:" << std::endl;

    std::cout << "Гистограмма для библиотечного генератора:" << std::endl;
    printHistogram(libraryHistogram, intervalSize); // Вывод гистограммы для библиотечных чисел

    std::cout << "Тест X-Square для библиотечного генератора:" << std::endl;
    testXSquare(libraryNumbers, quantityGeneratingNumbers, numBins, intervalSize); // Проведение теста X-Square для библиотечных чисел

    // Освободите выделенную память
    delete[] myNumbers;
    delete[] libraryNumbers;

    return 0;
}
