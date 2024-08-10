#ifndef DIPLOM_INITCALC_H
#define DIPLOM_INITCALC_H

#define noImp

#include "CalculationStep.h"
#include <optional>
#include "../math_core/Dipoles.h"
#include <omp.h>
#include <filesystem>

namespace inter {
    template<typename ... Args>
    class InitCalc : public CalculationStep<Args ...> {
    public:
        using CalculationStep<Args ...>::CalculationStep;

        std::string to_string() override {
            return "init_" + CalculationStep<Args ...>::to_string();
        }

        void setFunction(std::function<std::vector<double>(int, int, Args ...)> &&func) {
            source_ = std::forward<decltype(func)>(func);
        }

    protected:
        void perform_calculation(std::shared_ptr<DataAcessInteface> dat, Args...args) override {
            if (source_.has_value()) {
                std::vector<std::vector<double>> temp;//tododo floattype
                temp.resize(this->count_);
                for (int i = 0; i < this->count_; ++i) {
                    temp[i] = source_.value()(this->start_, i, args...);
                }
                dat->getdat(this->to_string()) = temp;
                this->notify(dat, args...);
                return;
            }
            std::vector<std::vector<double>> temp;//tododo floattype
            temp.resize(this->count_);
            for (int i = 0; i < this->count_; ++i) {
                temp[i] = std::vector<double>(this->start_, i);
            }
            dat->getdat(this->to_string()) = temp;
            this->notify(dat, args...);
        }

        std::optional<std::function<std::vector<double>(int, int, Args ...)>> source_;

    };

    static void createSubDirectory(const std::string &dirname, const std::string &subdirectory = "") {

        if (!std::filesystem::exists("results/")) {
            std::filesystem::create_directory("results/");
        }

        if (!subdirectory.empty() && !std::filesystem::exists("results/" + subdirectory)) {
            std::filesystem::create_directory("results/" + subdirectory);
        }

        if (!std::filesystem::exists(dirname)) {
            std::filesystem::create_directory(dirname);
        }
    }

    static std::fstream openOrCreateFile(std::string filename) {
        std::fstream appendFileToWorkWith;
        appendFileToWorkWith.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

        if (!appendFileToWorkWith) {
            appendFileToWorkWith.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
        }
        return appendFileToWorkWith;
    }


    static std::string getString(const std::string &dirname, std::string &&name, int i,
                                 std::string &&end)//todo variadik шаблон с произвольными параметрами
    {
        return dirname + name + "_i" + std::to_string(i) + "." + end;
    }

    template<typename ... Args>
    class CalculateMatrix : public CalculationStep<Args ...> {
    public:
        using CalculationStep<Args ...>::CalculationStep;

        std::string to_string() override {
            return "CalculateMatrix_" + CalculationStep<Args ...>::to_string();
        }

    protected:
        void perform_calculation(std::shared_ptr<DataAcessInteface> dat, Args...args) override {
            using dipoles::Dipoles;

            Dipoles<FloatType> d1;
            //clocks_[1].tik();
            bool state = get<bool>(dat->getProperty(
                    "prety_print"));//todo мы эти параметры должны откуда-то брать(чё-то типо конфигурации при создании dat
            size_t Nsym = get<int>(dat->getProperty("Nsym"));
#ifndef noImp
            std::vector<std::vector<double>> coords=dat->getdat(this->prev_->to_string());
            if (state)
                goto pp;
            {

//todo привести solutions к виду double[][]

#pragma omp parallel for default(none) shared(Nsym,solutions,coords) firstprivate(d1)
//#pragma omp parallel for default(shared)
                for (int i = 0; i < Nsym; ++i) {
                    d1.setNewCoordinates(coords[i]);
                    solutions[i] = d1.solve_();
                }
            }
            pp:
            for (int i = 0; i < Nsym; ++i) {
                d1.setNewCoordinates(coords[i]);

                solutions[i] = d1.solve_();//todo этот метода надо починить так, чтобы мы возвращали нужынй тип
                auto filename = getString(this->dir_.value(), "sim", i, "txt");
                auto fout = openOrCreateFile(filename);
                fout << "Итерация симуляции i = " << i << "\n\n";
                printCoordinates(fout,coords[i]);
                fout << "\n";
                printSolutionFormat1(fout,solutions[i]);
                fout << "\n";
                fout << "\n";
                fout.close();
            }
#endif
        }


    };

}

#endif //DIPLOM_INITCALC_H
