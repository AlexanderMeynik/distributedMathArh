//
// Created by Lenovo on 23.04.2024.
//


#include "TestRunner.h"

void TestRunner::createSubDirectory(const string &dirname, const string &subdirectory) {
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

void TestRunner::generateGaus(size_t N, size_t Ns,
                              double aRange) {//todo предавать генератор(абстраткный класс genrator) у него есть методы genrate
    this->N_ = N;
    this->Nsym_ = Ns;
    this->aRange_ = aRange;
    coords_.resize(Ns);
    CoordGenerator<double> genr(0, aRange);
    for (int i = 0; i < Nsym_; ++i) {
        coords_[i] = genr.generateCoordinates(N);
    }
}

TestRunner::TestRunner() {
    coords_ = std::vector<array<vector<FloatType>, 2>>();
    solutions_ = std::vector<solution>();
    subdir_ = std::nullopt;
    dir_ = std::nullopt;
    aRange_ = std::nullopt;
    Nsym_ = std::nullopt;
    N_ = std::nullopt;
}

void TestRunner::solve(bool print) {
    using dipoles::Dipoles;

    Dipoles<FloatType> d1;
    if (print)
        goto pp;
    {

#pragma omp parallel for default(none) shared(Nsym_,solutions_,coords_) firstprivate(d1)
//#pragma omp parallel for default(shared)
        for (int i = 0; i < Nsym_.value(); ++i) {
            d1.setNewCoordinates(coords_[i]);
            d1.solve_();
            solutions_[i] = d1.getSolution_();
        }
    }
    pp:
    for (int i = 0; i < Nsym_.value(); ++i) {
        d1.setNewCoordinates(coords_[i]);
        d1.solve_();
        solutions_[i] = d1.getSolution_();
        auto filename = getString(this->dir_.value(), "sim", i, "txt");
        auto fout = openOrCreateFile(filename);
        fout << "Итерация симуляции i = " << i << "\n\n";
        d1.printCoordinates(fout);
        fout << "\n";
        d1.printSolutionFormat1(fout);
        fout << "\n";
        fout << "\n";
        fout.close();
    }
}

std::string TestRunner::getString(const string &dirname, string &&name, int i, string &&end) {
    return dirname + name + "_i" + std::to_string(i) + "." + end;
}

std::fstream TestRunner::openOrCreateFile(std::string filename) {
    std::fstream appendFileToWorkWith;
    appendFileToWorkWith.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

    if (!appendFileToWorkWith) {
        appendFileToWorkWith.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
    }
    return appendFileToWorkWith;
}

void TestRunner::generateFunction(bool print) {
    using dipoles::Dipoles;

    Dipoles<FloatType> d1;
    MeshProcessor<FloatType> mesh;
    auto result = mesh.getMeshGliff();
    if (!print) {
#pragma omp parallel for default(none) shared(Nsym_,solutions_,result) firstprivate(d1,mesh)
        for (int i = 0; i < Nsym_.value(); ++i) {
            d1.getFullFunction(coords_[i],solutions_[i]);

            mesh.generateNoInt(d1.getI2function());

            mesh.generateNoInt(d1.getI2function());
            auto mesht = mesh.getMeshdec()[2];

#pragma omp critical
            {
                addMesh(result, mesht);
            }
        }

    } else {
        for (int i = 0; i < Nsym_.value(); ++i) {
            d1.getFullFunction(coords_[i],solutions_[i]);

            mesh.generateNoInt(d1.getI2function());
            auto mesht = mesh.getMeshdec()[2];

            addMesh(result, mesht);

            auto filename = getString(this->dir_.value(), "sim", i, "txt");
            auto fout = openOrCreateFile(filename);
            mesh.printDec(fout);
            mesh.plotSpherical(getString(this->dir_.value(), "sim", i, "png"));
            d1.plotCoordinates(getString(this->dir_.value(), "coord", i, "png"), aRange_.value());
            fout.close();
        }

        for (int i = 0; i < result.size(); ++i) {
            for (int j = 0; j < result[0].size(); ++j) {
                result[i][j] /= Nsym_.value();
            }
        }

        std::ofstream out1(dir_.value() + "avg.txt");
        out1 << "Значение  целевой функции усреднённой по " << Nsym_.value() << " симуляциям "
             << "для конфигураций, состоящих из " << N_.value() << " диполей\n";
        mesh.setMesh3(result);
        mesh.printDec(out1);
        mesh.plotSpherical(dir_.value() + "avg.png");
        out1.close();

    }
}
