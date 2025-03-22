#include "../include/plotingUtils.h"
#include "common/MeshCreator.h"

#include <matplot/matplot.h>
namespace ms=meshStorage;
namespace mp=matplot;
void plotCoordinates(const std::string &name, FloatType ar, const std::vector<FloatType> &xi) {
    auto vit=xi.begin();
    auto middle=vit+xi.size()/2;

    std::array<std::vector<FloatType>, 2> dumm=
            {
                    std::vector<FloatType>(vit,middle),
                    std::vector<FloatType>(middle,xi.end())
            };
    plotCoordinates(name,ar,dumm);

}

void plotFunction(const std::string &filename, const ms::MeshCreator &mesh) {
    auto ax = matplot::gca();
    auto data_arr=sphericalTransformation(mesh);
    ax->surf(ms::unflatten(data_arr[0],mesh.dimensions),
             ms::unflatten(data_arr[1],mesh.dimensions),
             ms::unflatten(data_arr[2],mesh.dimensions))
            ->lighting(true).primary(0.8f).specular(0.2f);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
    ax->view(213, 22);
    ax->xlim({-40, 40});
    ax->ylim({-40, 40});
    ax->zlim({0, 90});

    matplot::save(filename);
    ax.reset();

}

void plotCoordinates(const std::string &name, FloatType ar, std::array<std::vector<FloatType>, 2> &xi) {
    auto ax = matplot::gca();
    ax->scatter(xi[0], xi[1]);
    ax->xlim({-8 * ar, 8 * ar});
    ax->ylim({-8 * ar, 8 * ar});
    matplot::save(name);
    ax.reset();

}
