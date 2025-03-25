#include "plotingUtils.h"
#include "common/sharedDeclarations.h"
#include "common/commonTypes.h"
#include "common/Parsers.h"
#include "common/Printers.h"
#include "MeshPlot.h"
using namespace commonTypes;
using namespace printUtils;//todo do betetr
static inline std::string res_dir_path = "../../../res/";
static inline std::string subdir = res_dir_path.append("configdata7_25");
using coordType=std::vector<std::vector<FloatType >>;
using meshStorage::MeshCreator;
using ttype = MeshCreator;


std::vector<ttype> inline getMeshes() {

    std::vector<ttype> values;

    int NN;
    std::ifstream meshes(subdir + "/meshes.txt");
    meshes>>NN;
    values.reserve(NN);

    ioFormat a;
    meshes>>a;

    EFormat ef;
    meshes>>ef;

    for (int i = 0; i < NN; ++i) {

        auto m=printUtils::parseMeshFrom(meshes);
        values.push_back(std::move(m));
    }


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

using shared::FloatType;
int main(int argc, char **argv)
{
    qputenv("QSG_RHI_BACKEND", "opengl");
    QApplication app(argc, argv);



    auto res=getMeshes();



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

        auto & rr1=res[val];

        rr1.plotAndSave("plot.png",plotFunction);

        qw2->replot(rr1);
    });


    QWidget::connect(pb,&QPushButton::clicked,[&](){

        qw2->saveToFile();
    });
    sampleCountX=5;
    sampleCountZ=5;
    surf1();

    qw2->replot(res[0]);





    ww.show();


    return app.exec();
}
