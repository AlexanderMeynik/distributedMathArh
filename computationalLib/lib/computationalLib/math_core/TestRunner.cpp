#include "computationalLib/math_core/TestRunner.h"
#include "common/Printers.h"

void TestRunner::CreateSubDirectory(const std::string &dirname, const std::string &subdirectory) {
  if (!std::filesystem::exists("results/")) {
    std::filesystem::create_directory("results/");
  }

  if (!subdirectory.empty() && !std::filesystem::exists("results/" + subdirectory)) {
    std::filesystem::create_directory("results/" + subdirectory);
  }

  if (!std::filesystem::exists(dirname)) {
    std::filesystem::create_directory(dirname);
  }
}

TestRunner::TestRunner() {
  coords_ = std::vector<Coordinates>();
  // std::vector<array<vector<FloatType>, 2>>();
  solutions_ = std::vector<Solution>();
  subdir_ = std::nullopt;
  dir_ = std::nullopt;
  a_range_ = std::nullopt;
  nsym_ = std::nullopt;
  n_ = std::nullopt;
}

void TestRunner::Solve() {
  using dipoles::Dipoles;

  Dipoles d1;
  //clocks_[1].tik();
  if (inner_state_ != StateT::OPENMP_NEW)
    goto pp;
  {
#pragma omp parallel for default(none) shared(nsym_, solutions_, coords_) firstprivate(d1)
//#pragma omp parallel for default(shared)
    for (int i = 0; i < nsym_.value(); ++i) {
      d1.SetNewCoordinates(coords_[i]);
      solutions_[i] = d1.Solve<dipoles::EigenVec>();
    }
  }
  pp:
  for (int i = 0; i < nsym_.value(); ++i) {
    d1.SetNewCoordinates(coords_[i]);

    solutions_[i] = d1.Solve<dipoles::EigenVec>();
    auto filename = GetString(this->dir_.value(), "sim", i, "txt");
    auto fout = OpenOrCreateFile(filename);
    fout << "Итерация симуляции i = " << i << "\n\n";
    PrintCoordinates2(fout, coords_[i]);
    fout << "\n";
    PrintSolutionFormat1(fout, solutions_[i]);
    fout << "\n";
    fout << "\n";
    fout.close();
  }
  //clocks_[1].tak();
}

std::string TestRunner::GetString(const std::string &dirname, std::string &&name, int i, std::string &&end) {
  return dirname + name + "_i" + std::to_string(i) + "." + end;
}

std::fstream TestRunner::OpenOrCreateFile(std::string filename) {
  std::fstream append_file_to_work_with;
  append_file_to_work_with.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

  if (!append_file_to_work_with) {
    append_file_to_work_with.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
  }
  return append_file_to_work_with;
}

void TestRunner::GenerateFunction() {

  using dipoles::Dipoles;

  Dipoles d1;
  mesh_storage::MeshCreator mesh;
  mesh.ConstructMeshes();
  auto result = mesh.data_[2];

  if (inner_state_ == StateT::OPENMP_NEW) {
#pragma omp parallel for default(none) shared(nsym_, solutions_, result) firstprivate(d1, mesh)
    for (int i = 0; i < nsym_.value(); ++i) {
      d1.GetFullFunction(coords_[i], solutions_[i]);

      mesh.ApplyFunction(d1.GetI2Function());

      auto mesht = mesh.data_[2];

#pragma omp critical
      {
        mesh_storage::AddMesh(result, mesht);
      }
    }

  } else {
    for (int i = 0; i < nsym_.value(); ++i) {
      d1.GetFullFunction(coords_[i], solutions_[i]);

      mesh.ApplyFunction(d1.GetI2Function());
      auto mesht = mesh.data_[2];

      mesh_storage::AddMesh(result, mesht);

      auto filename = GetString(this->dir_.value(), "sim", i, "txt");
      auto fout = OpenOrCreateFile(filename);
      //mesh.printDec(fout);//use print mesh


      fout.close();
    }

    /*for (int i = 0; i < result.size(); ++i) {
        for (int j = 0; j < result[0].size(); ++j) {
            result[i][j] /= Nsym_.value();
        }
    }*/
    result /= nsym_.value();

    std::ofstream out1(dir_.value() + "avg.txt");
    out1 << "Значение  целевой функции усреднённой по " << nsym_.value() << " симуляциям "
         << "для конфигураций, состоящих из " << n_.value() << " диполей\n";
    mesh.data_[2] = result;
    // mesh.printDec(out1);

    out1.close();

  }
  //clocks_[2].tak();
}

TestRunner::TestRunner(size_t n, size_t ns, double a_range, std::string dirname, std::string subdir, StateT state) {
  n = n;
  nsym_ = ns;
  a_range_ = a_range;
  subdir_ = subdir;
  std::stringstream ss;
  ss << a_range << ".csv";
  std::string a_str = ss.str();
  a_str.erase(std::remove(a_str.begin(), a_str.end(), '+'), a_str.end());
  std::replace(a_str.begin(), a_str.end(), '-', '_');
  if (dirname.empty()) {
    dir_ = "results/" + subdir_.value() + "experiment_N=" + std::to_string(n) +
        "_Nsym=" + std::to_string(nsym_.value()) + "_a=" + a_str + "_mode=" +
        ENUM_TO_STR(inner_state_, kStateToStr) + "/";
  } else {
    dir_ = "results/" + subdir_.value() + dirname;
  }
  CreateSubDirectory(dir_.value(), subdir_.value());
  solutions_.resize(nsym_.value());
  coords_.resize(nsym_.value());
  inner_state_ = state;

}


