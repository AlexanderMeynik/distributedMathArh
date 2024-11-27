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




    void MeshProcessor2::generateMeshes(const MeshProcessor2::integrableFunction &func) {
        FloatType rr1 = this->rr;
        //todo inverted function argument order
        meshdec[2] = meshStorage::computeFunction(meshdec[0], meshdec[1], [&func, &rr1](FloatType x, FloatType y) {
            return meshStorage::integrateLambdaForOneVariable<61>(func, y, x, 0, rr1);
        });


        sphericalTransformation();
        updateSpans();
    }

    void MeshProcessor2::generateNoInt(const MeshProcessor2::directionGraph &func) {
        meshdec[2] = computeFunction(meshdec[1], meshdec[0], func);
        sphericalTransformation();
        updateSpans();
    }

    void MeshProcessor2::plotSpherical(std::string filename) {
        auto ax = gca();
        ax->surf(unflatten(meshsph[0],nums),
                 unflatten(meshsph[1],nums),
                 unflatten(meshsph[2],nums))
                ->lighting(true).primary(0.8f).specular(0.2f);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
        ax->view(213, 22);
        ax->xlim({-40, 40});
        ax->ylim({-40, 40});
        ax->zlim({0, 90});

        matplot::save(filename);
        ax.reset();
    }

    void MeshProcessor2::printDec(std::ostream &out) {
        out << "Функция I(phi,th)\n";
        out << "phi\\th\t\t";
        for (size_t i = 0; i < meshDecSpans[2].extent(0) - 1; ++i) {
            out << scientificNumber(meshDecSpans[1][std::array{i,0UL}], 5) << '\t';
        }

        out << scientificNumber(meshDecSpans[1][std::array{(meshDecSpans[2].extent(0) - 1),1UL}], 5)
        << '\n';

        for (size_t i = 0; i < meshDecSpans[2].extent(1); ++i) {
            auto phi = meshDecSpans[0][std::array{0UL,i}];
            out << scientificNumber(phi, 5) << "\t";
            for (size_t j = 0; j < meshDecSpans[2].extent(0) - 1; ++j) {
                out << scientificNumber(meshDecSpans[2][std::array{j,i}], 5) << "\t";
            }
            out << scientificNumber(meshDecSpans[2][std::array{meshdec[2].size() - 1,i}], 5) << "\n";
        }
    }

    void MeshProcessor2::setMesh3(meshStorageType &val) {
        meshdec[2]=val;
        sphericalTransformation();
        updateSpans();
    }

    void MeshProcessor2::initCoordMeshes() {
        std::array<meshStorageType , 2> meshgrid1 = myMeshGrid(
                myLinspace(philims[0], philims[1], nums[0]), myLinspace(thelims[0], thelims[1], nums[1]));
        meshdec[0] = meshgrid1[0];
        meshdec[1] = meshgrid1[1];
    }

    void MeshProcessor2::sphericalTransformation() {
        this->meshsph[0] = this->meshdec[0];
        this->meshsph[1] = this->meshdec[1];
        this->meshsph[2] = this->meshdec[2];

        this->meshsph[0] = this->meshsph[2] * sin(this->meshdec[1]) * cos(this->meshdec[0]);
        this->meshsph[1] = this->meshsph[2] * sin(this->meshdec[1]) * sin(this->meshdec[0]);
        this->meshsph[2] = this->meshsph[2] * cos(this->meshdec[1]);

    }
}