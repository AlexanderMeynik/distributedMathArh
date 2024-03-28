#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <eigen3/Eigen/Dense>
#include <random>

/*
int main(int argc, char* argv[]) {
 double alpha=1;
    int NN=10;
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, 1);
    std::vector<Eigen::Vector<double,2>> array(NN);

}*/

#include <iostream>
#include <random>
#include <cmath>
#include <fstream>
// Функция для генерации нормально распределенного случайного числа средствами преобразования Бокса-Мюллера


int main() {
    std::ofstream out("coord1.txt");
    // Задаем параметры распределения
    double mean = 0.0; // Среднее значение
    double stddev = 1.0; // Стандартное отклонение
    double a = 1.0; // Параметр а в распределении
    double sqrt2= sqrt(2);
    stddev*=sqrt2;
    // Инициализируем генератор случайных чисел
    std::random_device rd;
    std::mt19937 rng(rd());

    std::normal_distribution<double> distribution(mean, stddev);

    // Генерируем случайные числа и выводим их
    int num_samples = 100000; // Количество сэмплов для генерации
    for (int i = 0; i < num_samples; ++i) {
        double x = distribution(rng);
        double y =distribution(rng);
        //std::cout << "x_" << i << ": " << x << ", y_" << i << ": " << y << std::endl;
        out <<x <<"\t" << y << '\n';
        // Проверяем, что случайные числа находятся в пределах заданного квадрата
        /*if ((x * x + y * y) <= (a * a * M_PI)) {
            // Печатаем координаты, если точка входит в область

        } else {
            // Если точка не входит в область, генерируем новую
            --i;
        }*/
    }
    out.close();
    return 0;
}
