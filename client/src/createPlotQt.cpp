#include "plotingUtils.h"
#include "common/typeCasts.h"
#include "common/commonTypes.h"
#include "iolib/Parsers.h"
#include "MeshPlot.h"
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

#include <QtDataVisualization>
#include <QMainWindow>
#include <QApplication>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <common/printUtils.h>
#include <QPushButton>


//todo resolve driver issue https://github.com/microsoft/wslg/issues/1295
//todo
using shared::FloatType;
int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");
    QApplication app(argc, argv);



    auto res=testFixtureGetter(filename);



    QSurface3DSeries *series = new QSurface3DSeries;

    QMainWindow ww;
    auto geom=QGuiApplication::primaryScreen()->geometry();
    ww.resize(geom.width()/2,geom.height()/2);


    Q3DSurface *surface=new Q3DSurface;



    surface->addSeries(series);


    auto qw=QWidget::createWindowContainer(surface);
    auto qw2=new MeshPlot;

    auto lineEdit=new QSlider;
    lineEdit->setMaximum(500);
    lineEdit->setMinimum(2);

    auto lineEdit2=new QSlider;
    lineEdit2->setMaximum(res.size()-1);
    lineEdit2->setMinimum(0);

    surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);

    int sampleCountX = 500;
    int sampleCountZ = 500;

    auto pb=new QPushButton;


    auto ll=new QHBoxLayout;
    ll->addWidget(qw);
    ll->addWidget(lineEdit);
    ll->addWidget(qw2);
    ll->addWidget(lineEdit2);
    ll->addWidget(pb);
    ww.setCentralWidget(new QWidget);
    ww.centralWidget()->setLayout(ll);

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
    QWidget::connect(lineEdit2,&QSlider::valueChanged,[&](){

        auto val=lineEdit2->value();
        std::cout<<val<<'\n';

        auto & rr1=get<4>(res[val]).vals_;

        rr1.plotAndSave("plot.png",plotFunction);

        qw2->replot(rr1);
    });


    QWidget::connect(pb,&QPushButton::clicked,[&](){

        qw2->saveToFile();
    });
    sampleCountX=5;
    sampleCountZ=5;
    surf1();

    qw2->replot(get<4>(res[0]).vals_);





    ww.show();


    return app.exec();
}
