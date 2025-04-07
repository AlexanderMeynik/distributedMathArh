#include "common/MeshCreator.h"

namespace meshStorage {

    co::meshStorageType
    computeFunction(const co::meshStorageType &a, const co::meshStorageType &b, const co::directionGraph &func) {
        if (!a.size()||!b.size())
        {
            throw shared::MyException(fmt::format("Zero container sizes: a.size() = {},b.size() = {}", a.size(),b.size()));
        }

        if (a.size() != b.size()) {
            throw shared::mismatchedSizes(a.size(),b.size());
        }

        auto sz = a.size();
        co::meshStorageType result(sz);
        for (size_t i = 0; i < sz; ++i) {
            result[i] = func(a[i], b[i]);
        }
        return result;
    }

    FloatType getMeshDiffNorm(const co::meshStorageType &mesh1, const co::meshStorageType &mesh2) {
        return sqrt(pow((mesh1 - mesh2), 2).sum());
    }

    void addMesh(co::meshStorageType &a, const co::meshStorageType &b) {
        a += b;
    }


    void MeshCreator::applyFunction(const co::directionGraph &func) {
        data[2] = meshStorage::computeFunction(data[0], data[1], func);
    }




    meshArr<3> sphericalTransformation(const MeshCreator &oth) {
        meshArr<3> res;
        for (size_t i = 0; i < 3; ++i) {
            res[i] = oth.data[i];
        }


        res[0] = oth.data[2] * sin(oth.data[1]) * cos(oth.data[0]);
        res[1] = oth.data[2] * sin(oth.data[1]) * sin(oth.data[0]);
        res[2] = oth.data[2] * cos(oth.data[1]);
        return res;
    }

    MeshCreator::MeshCreator(bool construct):dimensions(defaultDims),limits(defaultLims)
    {


        if(construct)
        {
            data={{co::meshStorageType(dimensions[0] * dimensions[1]),
                  co::meshStorageType(dimensions[0] * dimensions[1]),
                  co::meshStorageType(dimensions[0] * dimensions[1])}};
        }
    }

    void MeshCreator::constructMeshes(const dimType &dims, const limType &lims) {
        this->dimensions = dims;
        this->limits = lims;

        auto phi = meshStorage::myLinspace<std::valarray>(limits[2], limits[3], dimensions[1]);
        auto theta = meshStorage::myLinspace<std::valarray>(limits[0], limits[1], dimensions[0]);
        std::array<std::valarray<FloatType>, 2> coords;
        for (size_t i = 0; i < 2; ++i) {
            coords[i] = meshStorage::myLinspace<std::valarray>(limits[2 * i], limits[2 * i + 1], dimensions[i]);
        }

        auto rrr = meshStorage::myMeshGrid(phi, theta);
        data[0] = rrr[0];
        data[1] = rrr[1];
    }

    void MeshCreator::constructMeshes(std::optional<dimType> dimOpt, std::optional<limType> limOpt) {
        constructMeshes(dimOpt.value_or(this->dimensions), limOpt.value_or(this->limits));
    }

    void MeshCreator::applyIntegrate(const commonTypes::integrableFunction &func, FloatType a, FloatType b) {
        this->applyFunction([&func, &a, b](FloatType x, FloatType y) {
            return integrateLambdaForOneVariable<61>(func, y, x, a, b);
        });
    }

    void printDec(const meshStorage::MeshCreator &mmesh, std::ostream &out) {
        auto ext0 = mmesh.dimensions[0];
        auto ext1 = mmesh.dimensions[1];

        out<<ext0<<'\t'<<ext1<<'\n';
        out << "Функция I(phi,th)\n";
        out << "phi\\th\t\t";
        std::vector<FloatType> row(ext0);
        for (size_t i = 0; i < ext0 - 1; ++i) {
            row[i]=mmesh.data[1][i*ext1];
            out << mmesh.data[1][i*ext1] << '\t';
        }
        row[ext0 - 1]= mmesh.data[1][(ext0-1)*ext1];
        out << mmesh.data[1][(ext0-1)*ext1]
            << '\n';

        std::vector<FloatType> col(ext1);
        for (size_t i = 0; i < ext1; ++i) {
            auto phi = mmesh.data[0][i];
            col[i]=phi;
            out << phi << "\t";
            for (size_t j = 0; j < ext0 - 1; ++j) {
                out << mmesh.data[2][j*ext1+i] << "\t";
            }
            out << mmesh.data[2][(ext0 - 1)*ext1+i] << "\n";
        }
    }

    co::meshDrawClass unflatten(const commonTypes::meshStorageType &mm, const dimType &dims) {
        auto res = co::meshDrawClass(dims[0], co::stdVec(dims[1], 0.0));

        for (size_t i = 0; i < dims[0]; ++i) {
            for (size_t j = 0; j < dims[1]; ++j) {
                res[i][j] = mm[i * dims[1] + j];
            }
        }

        return res;
    }


}