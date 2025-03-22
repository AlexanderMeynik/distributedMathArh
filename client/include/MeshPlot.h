

#include <Q3DSurface>
#include <QSurface3DSeries>
#include <QWidget>
#include <QHBoxLayout>
#include "common/MeshCreator.h"

using meshStorage::MeshCreator,meshStorage::meshArr;
using shared::FloatType;
#define FTMax std::numeric_limits<FloatType>::max()
#define FTMin std::numeric_limits<FloatType>::lowest()
/**
 * @b Surface plot widget for mesh
 */
class MeshPlot:public QWidget
{
Q_OBJECT
public:

    MeshPlot();
    /*
     * @b Constructs MeshPlot and the input mesh
     */
    MeshPlot(const MeshCreator&mesh);

    void replot(const MeshCreator&mesh)
    {
        emit dataChanged(mesh);
    }
    /**
     * @b Saves plot into file fname
     * @param fname
     */
    void saveToFile(const QString&fname=QString::fromStdString("plotQt.png"));
    signals:
        void dataChanged(const MeshCreator& mesh);
protected:

    void plot(const meshArr<3>&rr,const std::array<size_t,2>&dims);

    /**
     * @b Sets the most of style for visuzliztion
     */
    void setStyle();

    Q3DSurface *surface;

};