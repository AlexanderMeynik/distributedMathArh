#include  "common/Printers.h"
#include "network_shared/networkTypes.h"
#include "math_core/Dipoles.h"
#include "common/Generator.h"
#include "omp.h"
#include <algorithm>
#include "plotingUtils.h"
#include "../../computationalLib/test/fileHandler.h"
constexpr double kARange = 1e-6;
double  dev=kARange* sqrt(2);
std::string t1Name="testLs";
std::string t2Name="testIters2";

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

      ms.ConstructMeshes();


      StdValarr coordinates(2 *ts.N_);

      EigenVec sol;


      auto functor =generators::get_normal_generator(0.0,dev);

      std::generate(std::begin(coordinates), std::end(coordinates), functor);

      dipoles1.SetNewCoordinates(coordinates);

      sol = std::move(dipoles1.Solve());

      dipoles1.GetFullFunction(coordinates, sol);

      ms.ApplyFunction(dipoles1.GetI2Function());


      ms.PlotAndSave(fmt::format("{}/Plot{}.png",t1Name,ts.N_), PlotFunction);

      std::ofstream os (fmt::format("{}/data{}.txt",t1Name,ts.N_));

      print_utils::PrintMesh(os,ms,IoFormat::HUMAN_READABLE);
      os.close();

};


static auto  n_message_callback2 =
    [](size_t itercount) {



      using namespace print_utils;
      network_types::TestSolveParam ts;
      ts.experiment_id=0;
      ts.N_=10;
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





      sm.PlotAndSave(fmt::format("{}/Plot{}.png",t2Name,itercount), PlotFunction);

      std::ofstream os (fmt::format("{}/data{}.txt",t2Name,itercount));

      print_utils::PrintMesh(os,sm,IoFormat::HUMAN_READABLE);
      os.close();

    };

int main()
{

  file_utils::CreateDirIfNotPresent(t1Name);
  std::vector<size_t> sss={1,2,5,10,20,50,100,200,500,1000,2000};


  std::for_each(sss.begin(),sss.end(),n_message_callback);


  std::vector<size_t> sss2={1,2,5,10,100,1000,10000/*,100000,1000000*/};


  std::for_each(sss2.begin(),sss2.end(),n_message_callback2);


  file_utils::CreateDirIfNotPresent(t2Name);


}