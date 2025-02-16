#include "plotingUtils.h"
#include "common/typeCasts.h"
#include "common/commonTypes.h"
#include "iolib/Parsers.h"
using namespace commonTypes;
static inline std::string res_dir_path = "../../../res/";
static inline std::string filename = res_dir_path.append("config.txt");
static inline std::string subdir = filename.substr(0, filename.rfind('.')) + "data7_25";
using coordType=std::vector<std::vector<FloatType >>;
using meshStorage::MeshCreator;
using ttype = std::tuple<std::string, std::vector<FloatType>, Parser<matrixType>, Parser<EigenVec>, Parser<MeshCreator>>;


std::vector<ttype> inline testFixtureGetter(const std::string & file) {

    std::vector<ttype> values;
    auto avec= parseDipoleCoordinates<coordType>(file);

    std::ifstream sols(subdir + "/solutions.txt");
    std::ifstream matrixes(subdir + "/matrixes.txt");
    std::ifstream meshes(subdir + "/meshes.txt");
    values.reserve(avec.size());


    FloatType steps[2];
    meshes >> steps[0] >> steps[1];
    size_t NN;
    for (size_t i = 0; i < avec.size(); ++i) {
        matrixes >> NN;
        sols >> NN;
        meshes >> NN;

        ttype value;

        auto matr = Parser<matrixType >(NN);
        matrixes >> matr;

        auto solvv = Parser<EigenVec>(NN);
        sols >> solvv;

        Parser<MeshCreator> meshh;
        meshes >> meshh;

        values.emplace_back(std::to_string(i), avec[i], matr, solvv, meshh);
    }

    sols.close();
    matrixes.close();
    meshes.close();
    return values;
}
/*
int main()
{
    auto res=testFixtureGetter(filename);

    auto &rr=get<4>(res[0]).vals_;
    rr.plotAndSave("res.png",plotFunction);



    return 0;
}*/

#include <QtDataVisualization>

int main(int argc, char **argv)
{
    /*qputenv("QSG_RHI_BACKEND", "opengl");*/
    QGuiApplication app(argc, argv);

    Q3DSurface surface;
    surface.setFlags(surface.flags() ^ Qt::FramelessWindowHint);


    auto res=testFixtureGetter(filename);

    auto &rr1=get<4>(res[0]).vals_;

    auto dims=rr1.dimensions;
    auto rr=/*sphericalTransformation*/(rr1.data);

    QSurfaceDataArray *data = new QSurfaceDataArray;
    data->reserve(dims[0]); // Reserve space for rows

    for (int i = 0; i < dims[0]; ++i) {
        QSurfaceDataRow *row = new QSurfaceDataRow(dims[1]); // Create a new row for each iteration

        for (int j = 0; j < dims[1]; ++j) {
            int index = i * dims[1] + j;
            /*(*row)[j].setPosition( QVector3D(i,j,i+j)
                    *//*rr[0][index],
                    rr[1][index],
                    rr[2][index]*//*);*/

            (*row)[j].setPosition( QVector3D(
                    rr[0][index],
                    rr[1][index],
                    rr[2][index]));
        }
        *data<<row;
    }

    QSurface3DSeries *series = new QSurface3DSeries;
    series->dataProxy()->resetArray(data);
    surface.addSeries(series);

    surface.show();


    return app.exec();
}
