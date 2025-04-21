#pragma once

#include <functional>
#include "common/commonTypes.h"
#include "common/MeshCreator.h"

namespace ms = mesh_storage;


using common_types::FloatType;
namespace mesh_storage {
class MeshCreator;
}

/**
 * @b Plots 3d mesh as surface using matplot++ callbacks
 * @param filename
 * @param mesh
 */
void PlotFunction(const std::string &filename, const ms::MeshCreator &mesh);

/**
 * @b Plots 2d scatter Plot using matplot++ callbacks
 * @param name
 * @param ar
 * @param xi
 */
void PlotCoordinates(const std::string &name, FloatType ar, std::array<std::vector<FloatType>, 2> &xi);

/**
 * @b PlotCoordinates variant that uses 1d array for xy coordinates
 * @param name
 * @param ar
 * @param xi
 */
void PlotCoordinates(const std::string &name, FloatType ar, const std::vector<FloatType> &xi);
