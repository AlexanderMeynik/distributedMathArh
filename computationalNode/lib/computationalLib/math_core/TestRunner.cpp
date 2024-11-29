#include "computationalLib/math_core/TestRunner.h"
#include "iolib/Printers.h"
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

void TestRunner::generateCoords(Generator<FloatType> &gen) {

    //clocks_[0].tik();
    /*coords_.resize(Nsym_.value());
    for (int i = 0; i < Nsym_; ++i) {
        coords_[i] = gen.generate();
    }
    if(coords_[0][0].size()!=N_)//может ввести отдельнкю спецаилизацтю для данного случая
    {
        N_=coords_[0][0].size();
    }*///todo remove

    // clocks_[0].tak();
}


TestRunner::TestRunner() {
    coords_ = std::vector<coordinates>();
    // std::vector<array<vector<FloatType>, 2>>();
    solutions_ = std::vector<solution>();
    subdir_ = std::nullopt;
    dir_ = std::nullopt;
    aRange_ = std::nullopt;
    Nsym_ = std::nullopt;
    N_ = std::nullopt;
}

void TestRunner::solve() {
    using dipoles::Dipoles;

    Dipoles d1;
    //clocks_[1].tik();
    if (inner_state != state_t::openmp_new)
        goto pp;
    {
#pragma omp parallel for default(none) shared(Nsym_, solutions_, coords_) firstprivate(d1)
//#pragma omp parallel for default(shared)
        for (int i = 0; i < Nsym_.value(); ++i) {
            d1.setNewCoordinates(coords_[i]);
            solutions_[i] = d1.solve<dipoles::EigenVec>();
        }
    }
    pp:
    for (int i = 0; i < Nsym_.value(); ++i) {
        d1.setNewCoordinates(coords_[i]);

        solutions_[i] = d1.solve<dipoles::EigenVec>();
        auto filename = getString(this->dir_.value(), "sim", i, "txt");
        auto fout = openOrCreateFile(filename);
        fout << "Итерация симуляции i = " << i << "\n\n";
        printCoordinates(fout, coords_[i]);
        fout << "\n";
        printSolutionFormat1(fout, solutions_[i]);
        fout << "\n";
        fout << "\n";
        fout.close();
    }
    //clocks_[1].tak();
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

void TestRunner::generateFunction() {

    using dipoles::Dipoles;

    Dipoles d1;
    meshStorage::MeshCreator mesh;
    mesh.constructMeshes();
    auto result = mesh.data[2];
    //clocks_[2].tik();
    if (inner_state == state_t::openmp_new) {
#pragma omp parallel for default(none) shared(Nsym_, solutions_, result) firstprivate(d1, mesh)
        for (int i = 0; i < Nsym_.value(); ++i) {
            d1.getFullFunction_(coords_[i], solutions_[i]);

            mesh.applyFunction(d1.getI2function());

            auto mesht = mesh.data[2];

#pragma omp critical
            {
                meshStorage::addMesh(result, mesht);
            }
        }

    } else {
        for (int i = 0; i < Nsym_.value(); ++i) {
            d1.getFullFunction_(coords_[i], solutions_[i]);

            mesh.applyFunction(d1.getI2function());
            auto mesht = mesh.data[2];

            meshStorage::addMesh(result, mesht);

            auto filename = getString(this->dir_.value(), "sim", i, "txt");
            auto fout = openOrCreateFile(filename);
            /*mesh.printDec(fout);//todo redo
            mesh.plotSpherical(getString(this->dir_.value(), "sim", i, "png"));//todo переделать
            *///plotCoordinates(getString(this->dir_.value(), "coord", i, "png"), aRange_.value(),coords_[i]);

            fout.close();
        }

        /*for (int i = 0; i < result.size(); ++i) {
            for (int j = 0; j < result[0].size(); ++j) {
                result[i][j] /= Nsym_.value();
            }
        }*/
        result/=Nsym_.value();

        std::ofstream out1(dir_.value() + "avg.txt");
        out1 << "Значение  целевой функции усреднённой по " << Nsym_.value() << " симуляциям "
             << "для конфигураций, состоящих из " << N_.value() << " диполей\n";
        mesh.data[2]=result;
       /* mesh.printDec(out1);
        mesh.plotSpherical(dir_.value() + "avg.png");*///todo redo
        out1.close();

    }
    //clocks_[2].tak();
}


