#pragma once

#include <functional>
#include "common/commonTypes.h"

using commonTypes::FloatType;
namespace meshStorage {
    class MeshCreator;
}

/**
 * @b Plots 3d mesh as surface using matplot++ callbacks
 * @param filename
 * @param mesh
 */
void plotFunction(const std::string &filename, const meshStorage::MeshCreator &mesh);

/**
 * @b Plots 2d scatter plot using matplot++ callbacks
 * @param name
 * @param ar
 * @param xi
 */
void plotCoordinates(const std::string &name, FloatType ar, std::array<std::vector<FloatType>, 2> &xi);

/**
 * @b plotCoordinates variant that uses 1d array for xy coordinates
 * @param name
 * @param ar
 * @param xi
 */
void plotCoordinates(const std::string &name, FloatType ar, const std::vector<FloatType> &xi);
