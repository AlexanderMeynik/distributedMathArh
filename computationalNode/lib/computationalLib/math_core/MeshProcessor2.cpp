#include "computationalLib/math_core/MeshProcessor2.h"
namespace meshStorage
{

    meshStorageType computeFunction(const meshStorageType &a, const meshStorageType &b, const dipoles::directionGraph &func) {

        if(a.size()!=b.size())
        {
            return {};
        }

        auto sz=a.size();
        meshStorageType result(sz);
        for (size_t i = 0; i < sz; ++i) {
            result[i]=func(a[i],b[i]);
        }
        return result;
    }

    FloatType getMeshDiffNorm(const meshStorageType &mesh1, const meshStorageType &mesh2) {
        return sqrt(pow((mesh1 - mesh2),2).sum());
    }

    void addMesh(meshStorageType &a, const meshStorageType &b) {
        a+=b;
    }


    meshArr<3> MeshCreator::sphericalTransformation() {
        meshArr<dimCount+1> res;
        for (size_t i = 0; i < dimCount+1; ++i) {
            res[i]=data[i];
        }


        res[0] = this->data[2] * sin(this->data[1]) * cos(this->data[0]);
        res[1] = this->data[2] * sin(this->data[1]) * sin(this->data[0]);
        res[2] = this->data[2] * cos(this->data[1]);
        return res;
    }

    void MeshCreator::applyFunction(const dipoles::directionGraph &plot) {
        data[2]=meshStorage::computeFunction(data[0],data[1],plot);
        computeViews(2);
    }

    void MeshCreator::constructMeshes(const std::optional<std::array<size_t, 2>> dimenstion,
                                      const std::optional<std::array<FloatType, 4>> limit) {
        if(dimenstion.has_value())
        {
            this->dimensions=dimenstion.value();
        }
        if(limit.has_value())
        {
            this->limits=limit.value();
        }

        auto phi=meshStorage::myLinspace<std::valarray>(limits[2],limits[3],dimensions[1]);
        auto theta=meshStorage::myLinspace<std::valarray>(limits[0],limits[1],dimensions[0]);
        std::array<std::valarray<FloatType>,dimCount> coords;
        for (size_t i = 0; i <dimCount ; ++i) {
            coords[i]=meshStorage::myLinspace<std::valarray>(limits[2*i],limits[2*i+1],dimensions[i]);
        }

        auto rr=meshStorage::myMeshGrid(phi,theta);
        data[0]=rr[0];
        data[1]=rr[1];
        computeViews();
    }

    void MeshCreator::computeViews(int val) {
        if(val!=-1)
        {
            spans[val]=meshStorage::mdSpanType(&(data[val][0]),dimensions[0],
                                               dimensions[1]);
            return;
        }
        for (size_t i = 0; i < spans.size(); ++i) {
            spans[i]=meshStorage::mdSpanType(&(data[i][0]),dimensions[0],
                                             dimensions[1]);
        }
    }
}