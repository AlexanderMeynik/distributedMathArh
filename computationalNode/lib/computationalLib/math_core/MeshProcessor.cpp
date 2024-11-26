#include "computationalLib/math_core/MeshProcessor.h"

FloatType getMeshDiffNorm(const meshDrawClass &mesh1, const meshDrawClass &mesh2) {
    FloatType res = 0;
    for (size_t i = 0; i < mesh1.size(); ++i) {
        for (int j = 0; j < mesh1[0].size(); ++j) {
            res += pow(mesh1[i][j] - mesh2[i][j], 2);
        }
    }
    return sqrt(res);
}

void addMesh(meshDrawClass &a, meshDrawClass &b) {
    size_t size2 = a[0].size();
    for (int i = 0; i < a.size(); ++i) {
        for (int j = 0; j < size2; ++j) {
            a[i][j] += b[i][j];
        }
    }
}

std::pair<meshDrawClass, meshDrawClass> mymeshGrid(const floatVector &a, const floatVector &b) {
    meshDrawClass x_mesh(b.size(), floatVector(a.size()));
    meshDrawClass y_mesh(b.size(), floatVector(a.size()));
    for (size_t i = 0; i < b.size(); ++i) {
        x_mesh[i] = a;
        for (size_t j = 0; j < a.size(); ++j) {
            y_mesh[i][j] = b[i];
        }
    }

    return std::make_pair(x_mesh, y_mesh);
}


void MeshProcessor::initCoordMeshes() {
    std::pair<meshDrawClass, meshDrawClass> meshgrid1 = mymeshGrid(
            myLinspace(philims[0], philims[1], nums[0]), myLinspace(thelims[0], thelims[1], nums[1]));
    meshdec[0] = meshgrid1.first;
    meshdec[1] = meshgrid1.second;
}


void MeshProcessor::setMesh3(meshDrawClass &val) {
    meshdec[2] = val;
    sphericalTransformation();
}


void MeshProcessor::printDec(std::ostream &out) {
    out << "Функция I(phi,th)\n";
    out << "phi\\th\t\t";
    for (int i = 0; i < meshdec[2].size() - 1; ++i) {
        out << scientificNumber(meshdec[1][i][0], 5) << '\t';
    }
    out << scientificNumber(meshdec[1][meshdec[2].size() - 1][0], 5) << '\n';

    for (int i = 0; i < meshdec[2][0].size(); ++i) {
        auto phi = meshdec[0][0][i];
        out << scientificNumber(phi, 5) << "\t";
        for (int j = 0; j < meshdec[2].size() - 1; ++j) {
            out << scientificNumber(meshdec[2][j][i], 5) << "\t";
        }
        out << scientificNumber(meshdec[2][meshdec[2].size() - 1][i], 5) << "\n";
    }

}


void MeshProcessor::plotSpherical(std::string filename) {
    auto ax = gca();
    ax->surf(meshsph[0], meshsph[1], meshsph[2])
            ->lighting(true).primary(0.8f).specular(0.2f);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
    //surf(x, y, z);
    ax->view(213, 22);
    ax->xlim({-40, 40});
    ax->ylim({-40, 40});
    ax->zlim({0, 90});

    matplot::save(filename);
    ax.reset();
}

void MeshProcessor::generateNoInt(const directionGraph &func) {
    FloatType rr1 = this->rr;

    //meshdec[2] = transform(meshdec[0], meshdec[1], func);
    meshdec[2] = applyFunctionToVVD(meshdec[0], meshdec[1], func);
    // std::transform(meshdec[0].begin(), meshdec[0].end(), meshdec[1].begin(), meshdec[1].end(),std::back_inserter(meshdec[2]),func);
    sphericalTransformation();
}

void MeshProcessor::sphericalTransformation() {
    this->meshsph[0] = this->meshdec[0];
    this->meshsph[1] = this->meshdec[1];
    this->meshsph[2] = this->meshdec[2];
    for (int i = 0; i < this->meshsph[0].size(); ++i) {
        for (int j = 0; j < this->meshsph[0][0].size(); ++j) {
            this->meshsph[0][i][j] = this->meshsph[2][i][j] * sin(this->meshdec[1][i][j]) * cos(this->meshdec[0][i][j]);
            this->meshsph[1][i][j] = this->meshsph[2][i][j] * sin(this->meshdec[1][i][j]) * sin(this->meshdec[0][i][j]);
            this->meshsph[2][i][j] = this->meshsph[2][i][j] * cos(this->meshdec[1][i][j]);
        }
    }
}

void MeshProcessor::generateMeshes(const integrableFunction &func) {

    FloatType rr1 = this->rr;
    meshdec[2] = applyFunctionToVVD(meshdec[0], meshdec[1], [&func, &rr1](FloatType x, FloatType y) {
        return integrateFunctionBy1Val<61>(func, y, x, 0, rr1);
    });
    /*meshdec[2] = transform(meshdec[0], meshdec[1], [&func, &rr1](T x, T y) {
        return integrateFunctionBy1Val<T, 61>(func, y, x, 0, rr1);
    });*/

    sphericalTransformation();
}

