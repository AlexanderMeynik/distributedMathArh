#include "testingUtils/FileHandler.h"
#include "testingUtils/GoogleCommon.h"
#include <gtest/gtest.h>

using namespace file_utils;
class FileSystemUtilsTests:public ::testing::Test{
 public:
  virtual void SetUp() {
  }

  virtual void TearDown() {
    std::for_each(pathes_.begin(), pathes_.end(),
                  [&](const auto &path)
                  {
      if(path!=fs::current_path()&&fs::exists(path)) {
        DeleteEntry(path);
      }
    }
    );
  }

  const FsPath & AppendPath(FsPath && path)
  {
    return *pathes_.insert(path).first;
  }
 protected:
  std::set<FsPath> pathes_;
};

TEST_F(FileSystemUtilsTests,GetNormalAbs)
{
  auto path= AppendPath(fs::current_path());
  ASSERT_EQ(path,GetNormalAbs(path));
}


TEST_F(FileSystemUtilsTests,CreateDirExists)
{
  auto path= AppendPath(fs::current_path());
  ASSERT_FALSE(CreateDirIfNotPresent(path));
}


TEST_F(FileSystemUtilsTests,CreateDirNotExists)
{
  auto path= AppendPath(fs::current_path()/"someDir");

  EXPECT_FALSE(fs::exists(path));

  EXPECT_TRUE(CreateDirIfNotPresent(path));

  EXPECT_TRUE(DirectoryExists(path));
}


TEST_F(FileSystemUtilsTests,CreateFileExists)
{
  auto path= AppendPath(fs::current_path());
  ASSERT_FALSE(CreateFileIfNotPresent(path));
}


TEST_F(FileSystemUtilsTests, CreateFileIfNotExists)
{
  auto path= AppendPath(fs::current_path()/"someFile.txt");

  EXPECT_FALSE(fs::exists(path));

  EXPECT_TRUE(CreateFileIfNotPresent(path));

  EXPECT_TRUE(FileExists(path));
}


TEST_F(FileSystemUtilsTests, CreateFileForce)
{
  auto parent_dir=AppendPath(fs::current_path()/"someDir");
  auto path= AppendPath(parent_dir/"someFile.txt");

  EXPECT_FALSE(fs::exists(parent_dir));
  EXPECT_FALSE(fs::exists(path));

  EXPECT_TRUE(CreateFileIfNotPresent(path, true));

  EXPECT_TRUE(DirectoryExists(parent_dir));
  EXPECT_TRUE(FileExists(path));
}

TEST_F(FileSystemUtilsTests, CreateFile)
{
  auto parent_dir=AppendPath(fs::current_path()/"someDir");
  auto path= AppendPath(parent_dir/"someFile.txt");

  EXPECT_FALSE(fs::exists(parent_dir));
  EXPECT_FALSE(fs::exists(path));

  EXPECT_FALSE(CreateFileIfNotPresent(path));

  EXPECT_FALSE(DirectoryExists(parent_dir));
  EXPECT_FALSE(FileExists(path));
}