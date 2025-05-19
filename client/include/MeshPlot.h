#pragma once

#include <Q3DSurface>
#include <QSurface3DSeries>
#include <QWidget>
#include <QHBoxLayout>
#include "common/MeshCreator.h"

using mesh_storage::MeshCreator, mesh_storage::MeshArr;
using shared::FloatType;
#define FT_MAX std::numeric_limits<FloatType>::max()
#define FT_MIN std::numeric_limits<FloatType>::lowest()

///namespace for custom gui classes
namespace gui_objects {
/**
 * @brief Surface Plot widget for mesh
 * @todo add signal for single value changes?
 */
class MeshPlot : public QWidget {
 Q_OBJECT
 public:
  MeshPlot();
  /**
   * @brief Constructs MeshPlot and the input mesh
   */
  MeshPlot(const MeshCreator &mesh);

  void Replot(const MeshCreator &mesh) {
    emit DataChanged(mesh);
  }

  /**
   * @brief Saves Plot into file fname
   * @param fname
   */
  void SaveToFile(const QString &fname = QString::fromStdString("plotQt.png"));
 signals:

  void DataChanged(const MeshCreator &mesh);
 protected:
  /**
   * @brief Sets series data with the provided mesh data
   * @param rr
   * @param dims
   * @todo create method variant for const MeshCreator &
   */
  void Plot(const MeshArr<3> &rr, const std::array<size_t, 2> &dims);

  /**
   * @brief Sets the most of style for visualization
   */
  void SetStyle();

  Q3DSurface *surface_;
};
}