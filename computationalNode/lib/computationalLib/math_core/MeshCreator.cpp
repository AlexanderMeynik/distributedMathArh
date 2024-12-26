#include "computationalLib/math_core/MeshCreator.h"

namespace meshStorage
{

    co::meshStorageType computeFunction(const co::meshStorageType &a, const co::meshStorageType &b, const co::directionGraph &func) {
        if(!a.size())//todo create my exception type with error type and message
        {
            throw std::length_error("Invalid container sizes for input : a.size = "
            +std::to_string(a.size())+", b.size() = "+std::to_string(b.size()));
        }

        if(a.size()!=b.size())
        {
            throw std::length_error("Mismatched container sizes for input : a.size = "
                                    +std::to_string(a.size())+", b.size() = "+std::to_string(b.size()));
        }

        auto sz=a.size();
        co::meshStorageType result(sz);
        for (size_t i = 0; i < sz; ++i) {
            result[i]=func(a[i],b[i]);
        }
        return result;
    }

    FloatType getMeshDiffNorm(const co::meshStorageType &mesh1, const co::meshStorageType &mesh2) {
        return sqrt(pow((mesh1 - mesh2),2).sum());
    }

    void addMesh(co::meshStorageType &a, const co::meshStorageType &b) {
        a+=b;
    }




    void MeshCreator::applyFunction(const co::directionGraph &plot) {
        data[2]=meshStorage::computeFunction(data[0],data[1],plot);
        computeViews(2);
    }

    void MeshCreator::constructMeshes() {


        auto phi=meshStorage::myLinspace<std::valarray>(limits[2],limits[3],dimensions[1]);
        auto theta=meshStorage::myLinspace<std::valarray>(limits[0],limits[1],dimensions[0]);
        std::array<std::valarray<FloatType>,dimCount> coords;
        for (size_t i = 0; i <dimCount ; ++i) {
            coords[i]=meshStorage::myLinspace<std::valarray>(limits[2*i],limits[2*i+1],dimensions[i]);
        }

        auto rrr=meshStorage::myMeshGrid(phi,theta);
        data[0]=rrr[0];
        data[1]=rrr[1];
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

    meshArr<dimCount + 1> sphericalTransformation(const MeshCreator &oth) {
        meshArr<dimCount+1> res;
        for (size_t i = 0; i < dimCount+1; ++i) {
            res[i]=oth.data[i];
        }


        res[0] = oth.data[2] * sin(oth.data[1]) * cos(oth.data[0]);
        res[1] = oth.data[2] * sin(oth.data[1]) * sin(oth.data[0]);
        res[2] = oth.data[2] * cos(oth.data[1]);
        return res;
    }
}