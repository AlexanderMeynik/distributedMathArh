

#include "MeshPlot.h"

template<typename T> T next_power2(T value)
{
    --value;
    for(size_t i = 1; i < sizeof(T) * CHAR_BIT; i*=2)
        value |= value >> i;
    return value+1;
}
void MeshPlot::plot(const meshArr<3> &rr, const std::array<size_t, 2> &dims) {
    QSurfaceDataArray *data = new QSurfaceDataArray;
    FloatType minX = FTMax, maxX =FTMin;
    FloatType minY = FTMax, maxY = FTMin;
    FloatType minZ = FTMax, maxZ = FTMin;

    for (int i = 0; i < dims[0]; ++i) {
        QSurfaceDataRow *row = new QSurfaceDataRow(dims[1]);
        for (int j = 0; j < dims[1]; ++j) {
            int index = i * dims[1] + j;

            FloatType x = rr[1][index];
            FloatType y = rr[2][index];
            FloatType z = rr[0][index];

            minX = std::min(minX, x); maxX = std::max(maxX, x);
            minY = std::min(minY, y); maxY = std::max(maxY, y);
            minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);

            (*row)[j].setPosition(QVector3D(x, y, z));
        }
        data->append(row);
    }




        surface->axisX()->setRange(-40,40);
        surface->axisZ()->setRange(-40,40);
        surface->axisY()->setRange(0,90);

        surface->axisX()->setSegmentCount(8);
        surface->axisZ()->setSegmentCount(8);
        surface->axisY()->setSegmentCount(9);


        surface->scene()->activeCamera()->setXRotation(245);
        surface->scene()->activeCamera()->setYRotation(22);


    surface->seriesList().at(0)->dataProxy()->resetArray(data);
}

MeshPlot::MeshPlot(const MeshCreator &mesh):MeshPlot() {

    emit dataChanged(mesh);

}

MeshPlot::MeshPlot():QWidget() {

    surface = new Q3DSurface();

    surface->addSeries(new QSurface3DSeries);

    auto lay=new QHBoxLayout;
    lay->addWidget(QWidget::createWindowContainer(surface));

    this->setLayout(lay);

    connect(this, &MeshPlot::dataChanged, [&](const auto &a) {
        auto rr = sphericalTransformation(a);
        plot(rr, a.dimensions);
    });

    connect(surface->scene()->activeCamera(),&Q3DCamera::zoomLevelChanged,[&](auto deg)
    {
        auto cam=surface->scene()->activeCamera();
        auto x=cam->xRotation();
        auto y=cam->yRotation();
        std::cout<<x<<':'<<y<<'\t'<<cam->zoomLevel()<<'\n';
    });
    connect(surface->scene()->activeCamera(),&Q3DCamera::yRotationChanged,[&](auto deg)
    {
        auto cam=surface->scene()->activeCamera();
        auto x=cam->xRotation();
        auto y=cam->yRotation();
        std::cout<<x<<':'<<y<<'\t'<<cam->zoomLevel()<<'\n';
    });


    connect(surface->scene()->activeCamera(),&Q3DCamera::xRotationChanged,[&](auto deg)
    {
        auto cam=surface->scene()->activeCamera();
        auto x=cam->xRotation();
        auto y=cam->yRotation();
        std::cout<<x<<':'<<y<<'\t'<<cam->zoomLevel()<<'\n';
    });

    setStyle();

}

void MeshPlot::saveToFile(const QString &fname) {
    auto image=surface->renderToImage(4);

    QImage highResImage = image.scaled(image.width() * 2, image.height() * 2,
                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    highResImage.save(fname, nullptr,100);

}

void MeshPlot::setStyle() {


    QSurface3DSeries *series = surface->seriesList().at(0);

    surface->axisY()->setLabels({"Yaxis"});
    surface->axisX()->setLabels({"Xaxis"});
    surface->axisZ()->setLabels({"Zaxis"});
    surface->axisZ()->setReversed(true);

    surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);


    surface->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetIsometricRight);
    surface->scene()->activeCamera()->setZoomLevel(100);


    Q3DTheme *theme = surface->activeTheme();

    QFont sansSerifFont("Arial", 10);
    theme->setFont(sansSerifFont);
    theme->setLabelBackgroundEnabled(false);

    surface->axisX()->setLabelFormat("%.2f");
    surface->axisY()->setLabelFormat("%.2f");
    surface->axisZ()->setLabelFormat("%.2f");



    series->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);



    theme->setLabelTextColor(Qt::black);


    theme->setBackgroundColor(Qt::white);
    theme->setWindowColor(Qt::white);
    theme->setBackgroundEnabled(false);
    theme->setGridLineColor(QColor(64,64,64));


    QLinearGradient gradient;
    gradient.setColorAt(0.0, QColor(0, 0, 255, 255));
    gradient.setColorAt(0.5, QColor(0, 255, 0, 255));
    gradient.setColorAt(1.0, QColor(255, 0, 0, 255));

    series->setBaseGradient(gradient);

    series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    series->setFlatShadingEnabled(false);


    surface->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

}
