

#include "MeshPlot.h"

template<typename T>
T NextPower2(T value) {
  --value;
  for (size_t i = 1; i < sizeof(T) * CHAR_BIT; i *= 2)
    value |= value >> i;
  return value + 1;
}

void MeshPlot::Plot(const MeshArr<3> &rr, const std::array<size_t, 2> &dims) {
  QSurfaceDataArray *data = new QSurfaceDataArray;
  FloatType min_x = FT_MAX, max_x = FT_MIN;
  FloatType min_y = FT_MAX, max_y = FT_MIN;
  FloatType min_z = FT_MAX, max_z = FT_MIN;

  for (int i = 0; i < dims[0]; ++i) {
    QSurfaceDataRow *row = new QSurfaceDataRow(dims[1]);
    for (int j = 0; j < dims[1]; ++j) {
      int index = i * dims[1] + j;

      FloatType x = rr[1][index];
      FloatType y = rr[2][index];
      FloatType z = rr[0][index];

      min_x = std::min(min_x, x);
      max_x = std::max(max_x, x);
      min_y = std::min(min_y, y);
      max_y = std::max(max_y, y);
      min_z = std::min(min_z, z);
      max_z = std::max(max_z, z);

      (*row)[j].setPosition(QVector3D(x, y, z));
    }
    data->append(row);
  }

  surface_->axisX()->setRange(-40, 40);
  surface_->axisZ()->setRange(-40, 40);
  surface_->axisY()->setRange(0, 90);

  surface_->axisX()->setSegmentCount(8);
  surface_->axisZ()->setSegmentCount(8);
  surface_->axisY()->setSegmentCount(9);

  surface_->scene()->activeCamera()->setXRotation(245);
  surface_->scene()->activeCamera()->setYRotation(22);

  surface_->seriesList().at(0)->dataProxy()->resetArray(data);
}

MeshPlot::MeshPlot(const MeshCreator &mesh) : MeshPlot() {

  emit DataChanged(mesh);

}

MeshPlot::MeshPlot() : QWidget() {

  surface_ = new Q3DSurface();

  surface_->addSeries(new QSurface3DSeries);

  auto lay = new QHBoxLayout;
  lay->addWidget(QWidget::createWindowContainer(surface_));

  this->setLayout(lay);

  connect(this, &MeshPlot::DataChanged, [&](const auto &a) {
    auto rr = SphericalTransformation(a);
    Plot(rr, a.dimensions_);
  });

  connect(surface_->scene()->activeCamera(), &Q3DCamera::zoomLevelChanged, [&](auto deg) {
    auto cam = surface_->scene()->activeCamera();
    auto x = cam->xRotation();
    auto y = cam->yRotation();
    std::cout << x << ':' << y << '\t' << cam->zoomLevel() << '\n';
  });
  connect(surface_->scene()->activeCamera(), &Q3DCamera::yRotationChanged, [&](auto deg) {
    auto cam = surface_->scene()->activeCamera();
    auto x = cam->xRotation();
    auto y = cam->yRotation();
    std::cout << x << ':' << y << '\t' << cam->zoomLevel() << '\n';
  });

  connect(surface_->scene()->activeCamera(), &Q3DCamera::xRotationChanged, [&](auto deg) {
    auto cam = surface_->scene()->activeCamera();
    auto x = cam->xRotation();
    auto y = cam->yRotation();
    std::cout << x << ':' << y << '\t' << cam->zoomLevel() << '\n';
  });

  SetStyle();

}

void MeshPlot::SaveToFile(const QString &fname) {
  auto image = surface_->renderToImage(4);

  QImage high_res_image = image.scaled(image.width() * 2, image.height() * 2,
                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  high_res_image.save(fname, nullptr, 100);

}

void MeshPlot::SetStyle() {

  QSurface3DSeries *series = surface_->seriesList().at(0);

  surface_->axisY()->setLabels({"Yaxis"});
  surface_->axisX()->setLabels({"Xaxis"});
  surface_->axisZ()->setLabels({"Zaxis"});
  surface_->axisZ()->setReversed(true);

  surface_->setFlags(surface_->flags() ^ Qt::FramelessWindowHint);

  surface_->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetIsometricRight);
  surface_->scene()->activeCamera()->setZoomLevel(100);

  Q3DTheme *theme = surface_->activeTheme();

  QFont sans_serif_font("Arial", 10);
  theme->setFont(sans_serif_font);
  theme->setLabelBackgroundEnabled(false);

  surface_->axisX()->setLabelFormat("%.2f");
  surface_->axisY()->setLabelFormat("%.2f");
  surface_->axisZ()->setLabelFormat("%.2f");

  series->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);

  theme->setLabelTextColor(Qt::black);

  theme->setBackgroundColor(Qt::white);
  theme->setWindowColor(Qt::white);
  theme->setBackgroundEnabled(false);
  theme->setGridLineColor(QColor(64, 64, 64));

  QLinearGradient gradient;
  gradient.setColorAt(0.0, QColor(0, 0, 255, 255));
  gradient.setColorAt(0.5, QColor(0, 255, 0, 255));
  gradient.setColorAt(1.0, QColor(255, 0, 0, 255));

  series->setBaseGradient(gradient);

  series->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  series->setFlatShadingEnabled(false);

  surface_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

}
