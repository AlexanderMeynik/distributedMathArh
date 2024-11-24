
#include <iostream>

#include <vector>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <matplot/matplot.h>

#include "common/lib.h"
#include "computationalLib/math_core/MeshProcessor.h"
#include "computationalLib/math_core/Dipoles.h"

const std::size_t maxPrecision = std::numeric_limits<double>::digits;


using namespace dipoles;
const double l = 1E-7;
double eps = 0.01;//todo метод печати матрицы в диполи
//todo сравнивать штуки
int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::string dirname = "movemovemove/";
    std::filesystem::create_directory(dirname);
    std::array<std::vector<double>, 2> coordinates;
    double start_multip = 1 / 8.0;
    coordinates[0] = {0.0, start_multip * l};
    coordinates[1] = {0.0, start_multip * l};
    double limit = 0.01 * l;
    int i = 0;
    int N = coordinates[0].size();
    std::ofstream out(dirname + "results.txt");

    Dipoles d(N, coordinates);//todo old implementation used



    auto solut1 = d.solve<EigenVec>();

    MeshProcessor mmesh;

    d.getFullFunction_(coordinates, solut1);
    solut1[0] = 5;
    mmesh.generateNoInt(d.getI2function());
    auto prevMesh = mmesh.getMeshdec();
    prevMesh[2][0][0] = 10000000000;
    double multip = start_multip;
    double res = 5;
    while (i < 30 &&
           (solut1.head(N).norm() + solut1.tail(N).norm()) / 1000000 <
           res)//(coordinates[0]-coordinates[1]).norm()>limit)
    {
        std::ofstream fout(dirname + "out" + std::to_string(N) + "_iter" + std::to_string(i) + ".txt");
        d.setNewCoordinates(coordinates);
        auto solut2 = d.solve<EigenVec>();

        d.getFullFunction_(coordinates, solut2);
        mmesh.generateMeshes(d.getIfunction());//todo not working since inner function is bad

        //printToFile<double>(N, coordinates, d, dirname,i,2);
        //когда разницы между ними почти не будет оставновка
        //функция для подсчёта нормы от разницы 2 мешей принимет vector<vector<Tr>>&
        mmesh.printDec(out);
        std::vector<std::vector<double>> t1 = prevMesh[2];
        std::vector<std::vector<double>> t2 = mmesh.getMeshdec()[2];
        res = (solut1.head(N) - solut2.head(N)).norm() +
              (solut1.tail(N) - solut2.tail(N)).norm();//getMeshDiffNorm(t1,t2);
        out << res << "\n\n\n\n\n";
        Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");

        //fout<<"Координаты диполей\n";

        // fout<<coordinates[0][0]<<"\t"<<coordinates[1][0]<<"\n";
        //fout<<coordinates[0][1]<<"\t"<<coordinates[1][1]<<"\n";
        fout << "Номер итерации\tНорма разницы векторов решений\n";
        fout << i << "\t\t" << res << "\n";
        mmesh.plotSpherical(dirname + "out" + std::to_string(N) + "_iter" + std::to_string(i) + ".png");
        ++i;

        d.printMatrix(fout, CleanFmt);
        printCoordinates2(fout, coordinates);
        printSolution(fout, solut2, CleanFmt);


        mmesh.printDec(fout);

        coordinates[0][1] = coordinates[0][1] + multip * l;
        coordinates[1][1] = coordinates[1][1] + multip * l;
        solut1 = solut2;
        prevMesh = mmesh.getMeshdec();
        if (i % 4) {
            multip *= 2;
        }
        if (i == 29) {
            show();
        }

        fout.close();

    }
    out.close();
    /*for (int i = 0; i < avec.size(); ++i) {
        std::vector<Eigen::Vector<double,2>>a=avec[i];
        int N=a.size();
        Dipoles< double> d(N,a);
        d.solve_();
        auto solut2=d.getSolution_();
        printToFile<double>(N, a, d, dirname,i,verbose);
    }*/

    return 0;
}