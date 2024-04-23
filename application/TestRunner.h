//
// Created by Lenovo on 23.04.2024.
//

#ifndef DIPLOM_TESTRUNNER_H
#define DIPLOM_TESTRUNNER_H
#include <optional>
#include <filesystem>
#include <omp.h>
#include "lib.h"
#include <fstream>
class TestRunner {
//для возможности запуска цепочки методов нужно сделать паттерн декоратор
//инициализацию вещей стоит вынести в отдельные методы
//создать хэш таблицу(массив) для всех функций-этапов
//сериализатор для данных
public:
    TestRunner();
    void generateGaus(size_t N,size_t Ns,double aRange);
    void solve(bool print=false);
    void generateFunction(bool print=false);
    //todo инициализация перменных(подумать над интерфеском)
    //при первом тесте можно создавать бд с названием математического ядра под нужды пользаков
    //логика use if exists create if not(саму эту логику надо добавить в менеджер бд)

private:

    typedef double FloatType;
    typedef  std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2> solution;
    static void createSubDirectory(const std::string& dirname,const std::string& subdirectory="");
    static std::fstream openOrCreateFile(std::string filename);
    static std::string getString(const std::string &dirname,std::string &&name, int i, std::string &&end);
    std::vector<array<vector<FloatType>, 2>> coords_;
    std::vector<solution> solutions_;
    std::optional<std::string> subdir_;
    std::optional<std::string> dir_;
    std::optional<FloatType > aRange_;
    std::optional<int> N_;
    std::optional<int> Nsym_;


};


#endif //DIPLOM;_TESTRUNNER_H
