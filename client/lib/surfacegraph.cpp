

#include "surfacegraph.h"

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

            FloatType x = rr[0][index];
            FloatType y = rr[2][index];
            FloatType z = rr[1][index];

            minX = std::min(minX, x); maxX = std::max(maxX, x);
            minY = std::min(minY, y); maxY = std::max(maxY, y);
            minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);

            (*row)[j].setPosition(QVector3D(x, y, z));
        }
        data->append(row);
    }

    auto makeLims=[&](QValue3DAxis *axis,FloatType min,FloatType max)
    {



        auto p2=[](FloatType val) {
            auto sign=(val>=0)*2-1;
            auto pp=std::floor(std::log2(std::abs(val)));
            if(std::abs(val)==pow(2,pp))
            {
                return val;
            }
            else
            {
                return sign*pow(2,pp+1);
            }
        };
        auto ll=p2(max-min);
        auto ll1=p2(min);
        auto ll2=p2(max);
       /* long long ll= next_power2(std::abs((long long)(max-min)));
        long long ll1=next_power2(std::abs((long long)(min)));
        long long ll2=next_power2(std::abs((long long)(max)));*/

        constexpr auto makeLims=[](FloatType val,FloatType vv) {
            long long signMask = (val < 0) ? -1 : 1;
            return signMask*vv;
        };

        ll1=makeLims(min,ll1);
        ll2=makeLims(max,ll2);

        std::cout<<min<<'\t'<<max<<'\n';
        std::cout<<ll1<<'\t'<<ll2<<'\t'<<ll<<'\n';


        axis->setRange(min, max);
        axis->setSegmentCount(8);
    };

    if(axisRanges)
    {
        surface->axisX()->setRange(-40,40);
        surface->axisZ()->setRange(-40,40);
        surface->axisY()->setRange(0,90);

        surface->axisX()->setSegmentCount(8);
        surface->axisZ()->setSegmentCount(8);
        surface->axisY()->setSegmentCount(9);

        FloatType azimuth_deg = 213.0f;
        FloatType elevation_deg = 22.0f;
        FloatType az = qDegreesToRadians(azimuth_deg);
        FloatType el = qDegreesToRadians(elevation_deg);

        // Choose a suitable distance (adjust 'r' as needed for zoom level)
        FloatType r = 150.0f;

        FloatType x = r * std::cos(az) * std::cos(el);
        FloatType y = r * std::sin(el);
        FloatType z = r * std::sin(az) * std::cos(el);


        auto camera = surface->scene()->activeCamera();
        camera->setPosition(QVector3D(x, y, z));
        /*camera->setViewCenter(QVector3D(0, 0, 0));*/
       /* camera->setUpVector(QVector3D(0, 1, 0));*/
/*
        surface->scene()->activeCamera()->setXRotation(213);
        surface->scene()->activeCamera()->setYRotation(180-22);*/
    }
    else {


        makeLims(surface->axisX(), minX, maxX);
        makeLims(surface->axisY(), minY, maxY);
        makeLims(surface->axisZ(), minZ, maxZ);
    }



//todo play with some
   // surface->seriesList().at(0)->setDrawMode(QSurface3DSeries::DrawSurface);

    surface->seriesList().at(0)->dataProxy()->resetArray(data);
}

MeshPlot::MeshPlot(const MeshCreator &mesh):MeshPlot() {

    emit dataChanged(mesh);

}

MeshPlot::MeshPlot():QWidget(),axisRanges(true) {
    surface = new Q3DSurface;
    surface->addSeries(new QSurface3DSeries);
    surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);
    surface->axisY()->setLabels({"Yaxis"});
    surface->axisX()->setLabels({"Xaxis"});
    surface->axisZ()->setLabels({"Zaxis"});
    auto lay=new QHBoxLayout;
    lay->addWidget(QWidget::createWindowContainer(surface));

    this->setLayout(lay);

    connect(this, &MeshPlot::dataChanged, [&](const auto &a) {
        auto rr = sphericalTransformation(a);
        plot(rr, a.dimensions);
    });

//todo check
    connect(surface->scene()->activeCamera(),&Q3DCamera::zoomLevelChanged,[&](auto deg)
    {
        /*std::cout<<<<deg<<'\n';*/
        auto cam=surface->scene()->activeCamera();
        auto x=cam->xRotation();
        auto y=cam->yRotation();
        std::cout<<x<<':'<<y<<'\t'<<cam->zoomLevel()<<'\n';
    });
    connect(surface->scene()->activeCamera(),&Q3DCamera::yRotationChanged,[&](auto deg)
    {
        /*std::cout<<<<deg<<'\n';*/
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
}
