
#include <random>
#include <iostream>
#include <vector>

#include <matplot/matplot.h>

#include "computationalLib/math_core/MeshCreator.h"
#include "computationalLib/math_core/Dipoles.h"

#include "iolib/plotingUtils.h"
#include "iolib/Printers.h"

//template <class Tr>
std::array<std::vector<double>, 2> squareGrid(double l, double r, double a = 0, double b = 0, bool center = false) {
    //long double lim= ceil(r+r/l);
    long double lims[2] = {ceil(-r / l), floor(r / l)};

    long funccount = floor(r / l) - ceil(-r / l) + 1;

    if (center) {
        lims[0]--;
        funccount++;
    }


    std::vector<long double> x_1 = meshStorage::myLinspace(lims[0] * l, lims[1] * l, funccount);
    //std::vector<double>y= myLinspace(-lim[0]*l,-lim[1]*l,functiont);



    std::vector<double> x(funccount * funccount, 0);
    std::vector<double> y(funccount * funccount, 0);

    for (int i = 0; i < funccount; ++i) {
        for (int j = 0; j < funccount; ++j) {

            x[j + i * funccount] = x_1[j];
            y[j + i * funccount] = x_1[i];

            x[j + i * funccount] += a;
            y[j + i * funccount] += b;

        }
    }

    if (center) {
        for (int i = 0; i < funccount; ++i) {
            for (int j = 0; j < funccount; ++j) {

                x[j + i * funccount] += l / 2.0;
                y[j + i * funccount] += l / 2.0;

            }
        }
    }


    std::vector<double> x_filtered, y_filtered;

    for (int i = 0; i < funccount * funccount; ++i) {
        if (std::sqrt(pow(x[i] - a, 2) + pow(y[i] - b, 2)) < r) {
            x_filtered.push_back(x[i]);
            y_filtered.push_back(y[i]);
        }
    }
    std::cout << x_filtered.size() << '\t' << x.size() << '\t' << x_filtered.size() / (1.0 * x.size()) << "\n";
    return {x_filtered, y_filtered};
}

//todo implement generator
std::array<std::vector<double>, 2>
triangularGrid(double l, double r, double a = 0, double b = 0, bool center = false)//r-r,l-длинна
{
    long double c1 = 1 / 2.0;
    long double c2 = sqrt(3) / 2.0;
    double k = c2 / c1;

    double D = sqrt(k * k * r * r / (k * k + 1));//здесь мы находим x координаты для точек пересечения
    double x_t[2] = {-D, D};
    double y_t[2];
    for (int i = 0; i < 2; ++i) {
        y_t[i] = -(x_t[i]) / k;//подставляем их в исходный код
    }
    double c_t[2];

    for (int i = 0; i < 2; ++i) {
        c_t[i] = y_t[i] - x_t[i] * k;
    }
    int l_t[2];

    for (int i = 0; i < 2; ++i) {
        l_t[i] = -(c_t[i]) / (k * l);
    }





    //todo 2 круг симметричная фигура-> можно подсчитать результаты в 1 четверти
    //а что будет если поместить треугольники так, чробы центр центрального был в центре круга
    if (center) {
        l_t[0]--;
        //l_t[1];
    }
    int funccount = std::abs(l_t[1] - l_t[0]) + 1;
    std::vector<double> x1 = meshStorage::myLinspace(l_t[0] * l, l_t[1] * l, funccount);
    std::vector<double> y1(funccount, 0);

    std::vector<double> x(funccount * funccount, 0);
    std::vector<double> y(funccount * funccount, 0);

    for (int i = 0; i < funccount; ++i) {
        for (int j = 0; j < funccount; ++j) {

            x[j + i * funccount] = x1[i] / 2 + x1[j] / 2 + (c1 / c2) * (y1[i] + y1[j]) / 2;
            y[j + i * funccount] = (-c2 / c1) * x[j + i * funccount] + (c2 / c1) * x1[i] - y1[i];

            x[j + i * funccount] += a;
            y[j + i * funccount] += b;

        }
    }

    if (center) {
        for (int i = 0; i < funccount; ++i) {
            for (int j = 0; j < funccount; ++j) {

                x[j + i * funccount] += l / 2.0;
                y[j + i * funccount] += l * c2 / 2.0;

            }
        }
    }


    std::vector<double> x_filtered, y_filtered;

    for (int i = 0; i < funccount * funccount; ++i) {
        if (std::sqrt(pow(x[i] - a, 2) + pow(y[i] - b, 2)) < r) {
            x_filtered.push_back(x[i]);
            y_filtered.push_back(y[i]);
        }
    }
    std::cout << x_filtered.size() << '\t' << x.size() << '\t' << x_filtered.size() / (1.0 * x.size()) << "\n";
    return {x_filtered, y_filtered};
}

int main(int argc, char *argv[]) {
    //todo fix parameter calcultaion for array
    char *end;
    bool ssquare = strtol(argv[1], &end, 10);
    int N = 10;
    //std::cin>>N;
    using namespace Eigen;
    double a = 1e-6;
    double l = a / 10.0;
    std::cin >> l;
    l = a / l;
    bool center = true;
    std::cin >> center;
    auto coords = ((ssquare) ? squareGrid(l, a, 0, 0, center) : triangularGrid(l, a, 0, 0, center));
    N = coords[0].size();


    std::stringstream ss;
    ss << "_N" << N << "_l" << l << "_a" << a << "_center" << center;

    std::ofstream out("res" + ss.str() + ".txt");
    dipoles::Dipoles d(coords);
    auto solv = d.solve<dipoles::EigenVec>();

    plotCoordinates("coord" + ss.str() + ".png", a / 8, coords);

    printSolutionFormat1(out, solv);

    meshStorage::MeshCreator meshProcessor;
    meshProcessor.constructMeshes();
    d.getFullFunction_(coords, solv);

    meshProcessor.applyFunction(d.getI2function());

   // meshProcessor.plotSpherical("function" + ss.str() + ".png");
    //todo copy from generatoirs

    printCoordinates2(out, coords);
    //meshProcessor.printDec(out);//todo redo
    out.close();

}