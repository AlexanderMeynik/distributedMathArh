#include "../include/plotingUtils.h"
#include "common/MeshCreator.h"

#include <matplot/matplot.h>

namespace mp = matplot;

void PlotCoordinates(const std::string &name, FloatType ar, const std::vector<FloatType> &xi) {
  auto vit = xi.begin();
  auto middle = vit + xi.size() / 2;

  std::array<std::vector<FloatType>, 2> dumm =
      {
          std::vector<FloatType>(vit, middle),
          std::vector<FloatType>(middle, xi.end())
      };
  PlotCoordinates(name, ar, dumm);

}

void PlotFunction(const std::string &filename, const ms::MeshCreator &mesh) {
  auto ax = matplot::gca();
  auto data_arr = SphericalTransformation(mesh);
  ax->surf(ms::Unflatten(data_arr[0], mesh.dimensions_),
           ms::Unflatten(data_arr[1], mesh.dimensions_),
           ms::Unflatten(data_arr[2], mesh.dimensions_))
      ->lighting(true).primary(0.8f).specular(0.2f);
  ax->view(213, 22);
  ax->xlim({-40, 40});
  ax->ylim({-40, 40});
  ax->zlim({0, 90});

  matplot::save(filename);
  ax.reset();

}

void PlotCoordinates(const std::string &name, FloatType ar, std::array<std::vector<FloatType>, 2> &xi) {
  auto ax = matplot::gca();
  ax->scatter(xi[0], xi[1]);
  ax->xlim({-8 * ar, 8 * ar});
  ax->ylim({-8 * ar, 8 * ar});
  matplot::save(name);
  ax.reset();

}
