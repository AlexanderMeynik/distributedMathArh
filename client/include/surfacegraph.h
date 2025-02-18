

#include <Q3DSurface>
#include <QSurface3DSeries>
#include <QWidget>
#include <QHBoxLayout>
#include "plotUtils/MeshCreator.h"

using meshStorage::MeshCreator,meshStorage::meshArr;
using shared::FloatType;
#define FTMax std::numeric_limits<FloatType>::max()
#define FTMin std::numeric_limits<FloatType>::lowest()
class MeshPlot:public QWidget
{
Q_OBJECT
public:
    MeshPlot();
    MeshPlot(const MeshCreator&mesh);
    void replot(const MeshCreator&mesh)
    {
        emit dataChanged(mesh);
    }
    void saveToFile(const QString&fname)
    {
        //todo proper angle
        auto w=surface->renderToImage();
        QImage image =w;

        image.save("plotQt.png", "PNG");
    }
    signals:
        void dataChanged(const MeshCreator& mesh);
protected:

    void plot(const meshArr<3>&rr,const std::array<size_t,2>&dims);

    Q3DSurface *surface;
    bool axisRanges;

};