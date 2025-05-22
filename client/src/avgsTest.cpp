#include  "common/Printers.h"
#include "network_shared/networkTypes.h"
#include "math_core/Dipoles.h"
#include "common/Generator.h"
#include "omp.h"
#include <algorithm>
#include "plotingUtils.h"
#include "../../computationalLib/test/fileHandler.h"
#include "../../computationalLib/test/BenchmarkHandler.h"
constexpr double kARange = 1e-6;
double  dev=kARange* sqrt(2);
std::string t1Name="testLs2";
std::string t2Name="testIters3";

static auto  n_message_callback =
    [](size_t N_) {



      using namespace print_utils;
      network_types::TestSolveParam ts;
      ts.experiment_id=0;
      ts.N_=N_;
      ts.range={0,1};

      using namespace common_types;
      dipoles::Dipoles dipoles1;
      mesh_storage::MeshCreator ms;

      ms.ConstructMeshes(std::array{28ul, 100ul});


      StdValarr coordinates(2 *ts.N_);

      EigenVec sol;


      auto functor =generators::get_normal_generator(0.0,dev);

      std::generate(std::begin(coordinates), std::end(coordinates), functor);

      dipoles1.SetNewCoordinates(coordinates);

      sol = std::move(dipoles1.Solve());

      dipoles1.GetFullFunction(coordinates, sol);

      ms.ApplyFunction(dipoles1.GetI2Function());


      ms.PlotAndSave(fmt::format("{}/PlotN={}.png",t1Name,ts.N_), PlotFunction);

      std::ofstream os (fmt::format("{}/dataN={}.txt",t1Name,ts.N_));

      print_utils::PrintMesh(os,ms,IoFormat::HUMAN_READABLE);
      os.close();

};


static auto  n_message_callback2 =
    [](std::tuple<size_t ,size_t> arr) {


      auto[N,itercount]=arr;
      using namespace print_utils;
      network_types::TestSolveParam ts;
      ts.experiment_id=0;
      ts.N_=N;
      ts.range={0,1};

      using namespace common_types;
      dipoles::Dipoles dipoles1;

      mesh_storage::MeshCreator sm;

      sm.ConstructMeshes();
      auto &res=sm.data_[2];


      mesh_storage::MeshCreator ms;

      ms.ConstructMeshes();


      StdValarr coordinates(2 *ts.N_);

      EigenVec sol;


      auto functor =generators::get_normal_generator(0.0,dev);

      for (int i = 0; i < itercount; ++i) {
        std::generate(std::begin(coordinates), std::end(coordinates), functor);

        dipoles1.SetNewCoordinates(coordinates);

        sol = std::move(dipoles1.Solve());

        dipoles1.GetFullFunction(coordinates, sol);
        ms.ApplyFunction(dipoles1.GetI2Function());

        res+=ms.data_[2];
      }
      res/=itercount;





      sm.PlotAndSave(fmt::format("{}/dataN={}_iter={}.png",t2Name,N,itercount), PlotFunction);

      std::ofstream os (fmt::format("{}/dataN={}_iter={}.txt",t2Name,N,itercount));

      print_utils::PrintMesh(os,sm,IoFormat::HUMAN_READABLE);
      os.close();

    };

int main()
{

  file_utils::CreateDirIfNotPresent(t1Name);
  std::vector<size_t> sss={1,2,5,10,20,50,100,200,500,1000,2000};


  std::for_each(sss.begin(),sss.end(),n_message_callback);


  auto sss_1=std::array{/*1ul,2ul,5ul,10ul,20ul,50ul*/100ul,200ul/*,500ul*/};
  auto sss2=std::array{1ul,2ul,5ul,10ul,100ul,1000ul,10000ul,100000ul,/*1000000ul*/};
  auto comb=bench_utils::CartesianProduct(sss_1,sss2);

  /*std::for_each(comb.begin(),comb.end(),n_message_callback2);*/


  file_utils::CreateDirIfNotPresent(t2Name);


}