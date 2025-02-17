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


//todo printers
template<typename Struct>
void printBidir(const Struct &vec, std::ostream &out = std::cout, const char *delim = " ", const char *end = "\n") {
    if (vec.size()==0) {
        return;
    }
    auto beg = std::begin(vec);
    for (; std::next(beg) != std::end(vec); beg++) {
        out << *beg << delim;
    }
    out << *beg << end;
}

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

#include <QtDataVisualization>
#include <QMainWindow>
#include <QApplication>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <common/printUtils.h>
//todo resolve driver issue https://github.com/microsoft/wslg/issues/1295
int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");
    QApplication app(argc, argv);




    auto res=testFixtureGetter(filename);

    auto &rr1=get<4>(res[0]).vals_;

    auto dims=rr1.dimensions;

    auto rr=sphericalTransformation(rr1);

    printUtils::IosStateScientific ios(std::cout);

    std::cout<<" original data\n";
    printBidir(rr1.data[0]);
    std::cout<<"\n\n\n\n\n";
    printBidir(rr1.data[1]);
    std::cout<<"\n\n\n\n\n";
    printBidir(rr1.data[2]);
    std::cout<<"\n\n\n\n\n";

    std::cout<<" after spherical transformation\n";
    printBidir(rr[0]);
    std::cout<<"\n\n\n\n\n";
    printBidir(rr[1]);
    std::cout<<"\n\n\n\n\n";
    printBidir(rr[2]);
    std::cout<<"\n\n\n\n\n";




    QSurfaceDataArray *data = new QSurfaceDataArray;
    QSurface3DSeries *series = new QSurface3DSeries;
    QMainWindow ww;
    auto geom=QGuiApplication::primaryScreen()->geometry();
    ww.resize(geom.width()/2,geom.height()/2);


    Q3DSurface *surface=new Q3DSurface;
    auto qw=QWidget::createWindowContainer(surface);


    auto lineEdit=new QSlider;
    lineEdit->setMaximum(500);
    lineEdit->setMinimum(2);


    surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);
    int sampleCountX = 500;
    int sampleCountZ = 500;
    auto surf1=[&]()
    {

        int heightMapGridStepX = 6;
        int heightMapGridStepZ = 6;
        float sampleMin = -8.0f;
        float sampleMax = 8.0f;

        float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
        float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        dataArray->reserve(sampleCountZ);
        for (int i = 0 ; i < sampleCountZ ; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
            // Keep values within range bounds, since just adding step can cause minor drift due
            // to the rounding errors.
            float z = qMin(sampleMax, (i * stepZ + sampleMin));
            int index = 0;
            for (int j = 0; j < sampleCountX; j++) {
                float x = qMin(sampleMax, (j * stepX + sampleMin));
                float R = qSqrt(z * z + x * x) + 0.01f;
                float y = (qSin(R) / R + 0.24f) * 1.61f;
                (*newRow)[index++].setPosition(QVector3D(x, y, z));
            }
            *dataArray << newRow;
        }

        series->dataProxy()->resetArray(dataArray);
    };


    QWidget::connect(lineEdit,&QSlider::valueChanged,[&](){

        auto val=lineEdit->value();
        std::cout<<val<<'\n';
        sampleCountX=val;
        sampleCountZ=val;
        surf1();
    });

    sampleCountX=5;
    sampleCountZ=5;
    surf1();
    auto ll=new QHBoxLayout;
    ll->addWidget(qw);
    ll->addWidget(lineEdit);

    ww.setCentralWidget(new QWidget);
    ww.centralWidget()->setLayout(ll);


    /*surf1();*/
    /*data->reserve(dims[0]); // Reserve space for rows
     * for (int i = 0; i < dims[0]; ++i) {
        QSurfaceDataRow *row = new QSurfaceDataRow(dims[1]); // Create a new row for each iteration

        for (int j = 0; j < dims[1]; ++j) {
            int index = i * dims[1] + j;
            *//*(*row)[j].setPosition( QVector3D(i,j,i+j)
                    *//**//*rr[0][index],
                    rr[1][index],
                    rr[2][index]*//**//*);*//*

            (*row)[j].setPosition( QVector3D(
                    rr[0][index],
                    rr[1][index],
                    rr[2][index]));
        }
        *data<<row;
    }*/

    surface->addSeries(series);

    ww.show();


    return app.exec();
}
