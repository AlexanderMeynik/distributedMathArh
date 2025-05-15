#include "plotingUtils.h"
#include "common/sharedDeclarations.h"
#include "common/commonTypes.h"
#include "common/Parsers.h"
#include "common/Printers.h"
#include "MeshPlot.h"
using namespace gui_objects;
using namespace common_types;
using namespace print_utils;
static inline std::string res_dir_path = "../../../res/";
static inline std::string subdir = res_dir_path.append("configdata7_25");
using CoordType = std::vector<std::vector<FloatType >>;
using mesh_storage::MeshCreator;
using Ttype = MeshCreator;

std::vector<Ttype> inline GetMeshes() {

  std::vector<Ttype> values;

  int nn;
  std::ifstream meshes(subdir + "/meshes.txt");
  meshes >> nn;
  values.reserve(nn);

  IoFormat a;
  meshes >> a;

  EFormat ef;
  meshes >> ef;

  for (int i = 0; i < nn; ++i) {

    auto m = print_utils::ParseMeshFrom(meshes);
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

using shared::FloatType;

int main(int argc, char **argv) {
  qputenv("QSG_RHI_BACKEND", "opengl");
  QApplication app(argc, argv);

  auto res = GetMeshes();

  QSurface3DSeries *series = new QSurface3DSeries;

  QMainWindow ww;
  auto geom = QGuiApplication::primaryScreen()->geometry();
  ww.resize(geom.width() / 2, geom.height() / 2);

  Q3DSurface *surface = new Q3DSurface;

  surface->addSeries(series);

  auto qw = QWidget::createWindowContainer(surface);
  auto qw2 = new MeshPlot;

  auto line_edit = new QSlider;
  line_edit->setMaximum(500);
  line_edit->setMinimum(2);

  auto line_edit_2 = new QSlider;
  line_edit_2->setMaximum(res.size() - 1);
  line_edit_2->setMinimum(0);

  surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);

  int sample_count_x = 500;
  int sample_count_z = 500;

  auto pb = new QPushButton;

  auto ll = new QHBoxLayout;
  ll->addWidget(qw);
  ll->addWidget(line_edit);
  ll->addWidget(qw2);
  ll->addWidget(line_edit_2);
  ll->addWidget(pb);
  ww.setCentralWidget(new QWidget);
  ww.centralWidget()->setLayout(ll);

  auto surf1 = [&]() {

    int height_map_grid_step_x = 6;
    int height_map_grid_step_z = 6;
    float sample_min = -8.0f;
    float sample_max = 8.0f;

    float step_x = (sample_max - sample_min) / float(sample_count_x - 1);
    float step_z = (sample_max - sample_min) / float(sample_count_z - 1);

    QSurfaceDataArray *data_array = new QSurfaceDataArray;
    data_array->reserve(sample_count_z);
    for (int i = 0; i < sample_count_z; i++) {
      QSurfaceDataRow *new_row = new QSurfaceDataRow(sample_count_x);

      float z = qMin(sample_max, (i * step_z + sample_min));
      int index = 0;
      for (int j = 0; j < sample_count_x; j++) {
        float x = qMin(sample_max, (j * step_x + sample_min));
        float r = qSqrt(z * z + x * x) + 0.01f;
        float y = (qSin(r) / r + 0.24f) * 1.61f;
        (*new_row)[index++].setPosition(QVector3D(x, y, z));
      }
      *data_array << new_row;
    }

    series->dataProxy()->resetArray(data_array);
  };

  QWidget::connect(line_edit, &QSlider::valueChanged, [&]() {

    auto val = line_edit->value();
    std::cout << val << '\n';
    sample_count_x = val;
    sample_count_z = val;
    surf1();
  });
  QWidget::connect(line_edit_2, &QSlider::valueChanged, [&]() {

    auto val = line_edit_2->value();
    std::cout << val << '\n';

    auto &rr1 = res[val];

    rr1.PlotAndSave("Plot.png", PlotFunction);

    qw2->Replot(rr1);
  });

  QWidget::connect(pb, &QPushButton::clicked, [&]() {

    qw2->SaveToFile();
  });
  sample_count_x = 5;
  sample_count_z = 5;
  surf1();

  qw2->Replot(res[0]);

  ww.show();

  return app.exec();
}
