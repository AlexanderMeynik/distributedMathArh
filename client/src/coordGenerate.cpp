#include <iostream>
#include <fstream>

#include <random>

#include <omp.h>

#include <filesystem>

#include "math_core/Dipoles.h"
#include "common/Generator.h"
#include "common/MeshCreator.h"
#include "common/Printers.h"
#include "../include/plotingUtils.h"
#include "common/sharedDeclarations.h"

std::string GetString(const std::string &dirname, std::string &&name, int i, std::string &&end) {
  return dirname + name + "_i" + std::to_string(i) + "." + end;
}

using namespace shared;
using dipoles::Dipoles;
using print_utils::PrintCoordinates2, print_utils::PrintSolutionFormat1;

int main(int argc, char *argv[]) {
  int n = 5;
  int nsym = 1000;
  double a_range = 1e-6;
  int print = false;
  StateT state = StateT::OPENMP_NEW;
  std::string subdirectory = "";
  if (argc >= 3) {
    char *end;
    n = std::strtol(argv[1], &end, 10);
    nsym = std::strtol(argv[2], &end, 10);
  }
  if (argc >= 4) {
    char *end;
    a_range = std::strtod(argv[3], &end);
  }

  if (argc >= 5) {
    char *end;

    std::string mode = argv[4];

    state = kStringToState.find(mode)->second;


    //print=strtol(argv[4],&end,10);
  }
  if (argc >= 6) {
    subdirectory = argv[5];
    subdirectory += '/';
  }

  std::stringstream ss;
  ss << a_range << ".csv";
  std::string a_str = ss.str(); // filename = 1e+16.csv
  a_str.erase(std::remove(a_str.begin(), a_str.end(), '+'), a_str.end());// removing the '+' sign
  std::replace(a_str.begin(), a_str.end(), '-', '_');//, aStr.end());// removing the '-' sign
  //std::cout<<aStr; // 1e16.csv
  std::string dirname = "results/" + subdirectory + "experiment_N=" + std::to_string(n) +
      "_Nsym=" + std::to_string(nsym) + "_a=" + a_str + "_mode=" +
      kStateToStr[static_cast<size_t>(state)] + "/";
  if (!std::filesystem::exists("results/")) {
    std::filesystem::create_directory("results/");
  }

  if (subdirectory.size() && !std::filesystem::exists("results/" + subdirectory)) {
    std::filesystem::create_directory("results/" + subdirectory);
  }

  if (!std::filesystem::exists(dirname)) {
    std::filesystem::create_directory(dirname);
  }

  /*CoordGenerator<double> genr(0, aRange);*/
  std::vector<std::vector<double>> coordinates(nsym, std::vector<double>(2 * n));
  auto normal_gen = generators::get_normal_generator(0.0, a_range * sqrt(2));
  for (int i = 0; i < nsym; ++i) {
    std::generate(coordinates[i].begin(), coordinates[i].end(), normal_gen);
  }
  Dipoles dipoles1(coordinates[0]);
  using mesh_storage::MeshCreator;
  MeshCreator mesh = MeshCreator();
  mesh.ConstructMeshes();
  auto result = mesh.data_[2];

  Eigen::IOFormat clean_fmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
  std::vector<double> total_time(omp_get_max_threads(), 0);
  std::vector<double> function_time(omp_get_max_threads(), 0);
  std::vector<double> solve_time(omp_get_max_threads(), 0);

  double total_time_s = 0;
  double function_time_s = 0;
  double solve_time_s = 0;

  double stime = omp_get_wtime();

  switch (state) {
    case StateT::OPENMP_NEW:goto run_new_openmp;
      break;
    case StateT::NEW:goto run_new_;
      break;
    case StateT::OPENMP_OLD:goto run_old_openmp;
      break;
    case StateT::OLD:goto run_old_;
      break;
    case StateT::PRINT:goto print_;
      break;
  }
  //if(!print) {
  {
    run_new_openmp:
    stime = omp_get_wtime();
    //solveTime= ;
#pragma omp parallel for firstprivate(dipoles1, mesh), default(shared)
    for (int i = 0; i < nsym; ++i) {
      int tid = omp_get_thread_num();
      double stmep[2] = {omp_get_wtime(), 0};
      dipoles1.SetNewCoordinates(coordinates[i]);
      auto solution = dipoles1.Solve<dipoles::EigenVec>();
      dipoles1.GetFullFunction(coordinates[i], solution);
      stmep[1] = omp_get_wtime();
      solve_time[tid] += stmep[1] - stmep[0];

      double stmep2[2] = {omp_get_wtime(), 0};
      //stmep= omp_get_wtime();
      mesh.ApplyFunction(dipoles1.GetI2Function());
      auto mesht = mesh.data_[2];
      stmep2[1] = omp_get_wtime();
      function_time[tid] += stmep2[1] - stmep2[0];

#pragma omp critical
      {
        mesh_storage::AddMesh(result, mesht);
        //std::cout<<tid<<"\t"<<functionTime[tid]<<'\t'<<solveTime[tid]<<'\n';
      }
      total_time[tid] += omp_get_wtime() - stmep[0];
    }

    solve_time_s = std::accumulate(solve_time.begin(), solve_time.end(), 0.0) / omp_get_max_threads();
    function_time_s = std::accumulate(function_time.begin(), function_time.end(), 0.0) / omp_get_max_threads();
    total_time_s = std::accumulate(total_time.begin(), total_time.end(), 0.0) / omp_get_max_threads();
    goto printtimes;
  }

  {
    run_new_:
    stime = omp_get_wtime();
    //solveTime= ;
    for (int i = 0; i < nsym; ++i) {
      double stmep[2] = {omp_get_wtime(), 0};
      dipoles1.SetNewCoordinates(coordinates[i]);
      auto solution = dipoles1.Solve<dipoles::EigenVec>();
      dipoles1.GetFullFunction(coordinates[i], solution);
      stmep[1] = omp_get_wtime();
      solve_time_s += stmep[1] - stmep[0];

      double stmep2[2] = {omp_get_wtime(), 0};
      mesh.ApplyFunction(dipoles1.GetI2Function());
      auto mesht = mesh.data_[2];
      stmep2[1] = omp_get_wtime();
      function_time_s += stmep2[1] - stmep2[0];
      mesh_storage::AddMesh(result, mesht);
      total_time_s += omp_get_wtime() - stmep[0];
    }
    goto printtimes;
  }

  {
    run_old_:
    stime = omp_get_wtime();
    //solveTime= ;
    for (int i = 0; i < nsym; ++i) {
      double stmep[2] = {omp_get_wtime(), 0};
      dipoles1.SetNewCoordinates(coordinates[i]);
      auto solution = dipoles1.Solve<dipoles::EigenVec>();
      dipoles1.GetFullFunction(coordinates[i], solution);
      stmep[1] = omp_get_wtime();
      solve_time_s += stmep[1] - stmep[0];

      double stmep2[2] = {omp_get_wtime(), 0};
      mesh.ApplyIntegrate(dipoles1.GetIfunction());
      auto mesht = mesh.data_[2];
      stmep2[1] = omp_get_wtime();
      function_time_s += stmep2[1] - stmep2[0];
      mesh_storage::AddMesh(result, mesht);
      total_time_s += omp_get_wtime() - stmep[0];
    }
    goto printtimes;
  }

  {
    run_old_openmp:
    stime = omp_get_wtime();
    //solveTime= ;
#pragma omp parallel for private(dipoles1, mesh), default(shared)
    for (int i = 0; i < nsym; ++i) {
      int tid = omp_get_thread_num();
      double stmep[2] = {omp_get_wtime(), 0};
      dipoles1.SetNewCoordinates(coordinates[i]);
      auto solution = dipoles1.Solve<dipoles::EigenVec>();
      dipoles1.GetFullFunction(coordinates[i], solution);
      stmep[1] = omp_get_wtime();
      solve_time[tid] += stmep[1] - stmep[0];

      double stmep2[2] = {omp_get_wtime(), 0};
      //stmep= omp_get_wtime();
      mesh.ApplyIntegrate(dipoles1.GetIfunction());
      auto mesht = mesh.data_[2];
      stmep2[1] = omp_get_wtime();
      function_time[tid] += stmep2[1] - stmep2[0];

#pragma omp critical
      {
        mesh_storage::AddMesh(result, mesht);
        //std::cout<<tid<<"\t"<<functionTime[tid]<<'\t'<<solveTime[tid]<<'\n';
      }
      total_time[tid] += omp_get_wtime() - stmep[0];
    }

    solve_time_s = std::accumulate(solve_time.begin(), solve_time.end(), 0.0) / omp_get_max_threads();
    function_time_s = std::accumulate(function_time.begin(), function_time.end(), 0.0) / omp_get_max_threads();
    total_time_s = std::accumulate(total_time.begin(), total_time.end(), 0.0) / omp_get_max_threads();
    goto printtimes;
  }



  //}
  //else
  //{
  print_:
  for (int i = 0; i < nsym; ++i) {
    std::ofstream out1(GetString(dirname, "sim", i, "txt"));
    dipoles1.SetNewCoordinates(coordinates[i]);
    auto solution = dipoles1.Solve<dipoles::EigenVec>();
    //dipoles1.getFullFunction();
    dipoles1.GetFullFunction(coordinates[i], solution);
    mesh.ApplyFunction(dipoles1.GetI2Function());
    auto mesht = mesh.data_[2];
    {
      mesh_storage::AddMesh(result, mesht);
    }
    out1 << "Итерация симуляции i = " << i << "\n\n";
    PrintCoordinates2(out1, coordinates[i]);
    out1 << "\n";

    PrintSolutionFormat1(out1, solution);
    out1 << "\n";

    out1 << "\n";

    PrintDec(mesh, out1);

    out1.close();
    mesh.PlotAndSave(GetString(dirname, "sim", i, "png"), PlotFunction);
    PlotCoordinates(GetString(dirname, "coord", i, "png"), a_range, coordinates[i]);

  }
  goto printtimes;
  //}

  printtimes:
  double resulting_time = omp_get_wtime() - stime;
  //std::cout<<"Execution_time = "<<resulting_time<<"\tN = "<<N<<"\n";

  /*std::vector<double> times={std::accumulate(solveTime.begin(), solveTime.end(),0.0)/omp_get_max_threads(),//,[](double a,double b){return a+b;}),
                             std::accumulate(functionTime.begin(), functionTime.end(),0.0)/omp_get_max_threads(),
                             std::accumulate(totalTime.begin(), totalTime.end(),0.0)/omp_get_max_threads()
                             };*/
  std::cout << resulting_time << '\t' << solve_time_s
            << '\t' << function_time_s << '\t' <<
            total_time_s << '\t' << n << '\t';

  /*for (int i = 0; i < result.size(); ++i) {
      for (int j = 0; j < result[0].size(); ++j) {
          result[i][j] /= Nsym;
      }
  }*/
  result /= nsym;
  std::ofstream out1(dirname + "avg.txt");
  out1 << "Значение  целевой функции усреднённой по " << nsym << " симуляциям "
       << "для конфигураций, состоящих из " << n << " диполей\n";

  mesh.data_[2] = result;
  PrintDec(mesh, out1);
  mesh.PlotAndSave(dirname + "avg.png", PlotFunction);
  out1.close();

  return 0;
}


