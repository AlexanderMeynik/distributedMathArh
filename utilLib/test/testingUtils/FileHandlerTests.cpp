#include "testingUtils/FileHandler.h"
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



TEST_F(FileSystemUtilsTests, ReadFileToString)
{
  auto path=AppendPath(fs::current_path()/"someDir.txt");
  std::ofstream os(path);

  std::string ss="Some multiline \n string!";

  os<<ss;
  os.close();
  EXPECT_EQ(ReadFileToString(path),ss);
}

TEST_F(FileSystemUtilsTests, ReadFileToStringFileDoesntExist)
{
  auto path=AppendPath(fs::current_path()/"someDir.txt");
  EXPECT_FALSE(FileExists(path));
  EXPECT_EQ(ReadFileToString(path),"");
}

class FileHandlerTests:public FileSystemUtilsTests
{
 public:

  static void SetUpTestSuite() {
    file_handler_=std::make_unique<FileHandler>(fs::current_path()/"someDir");
  }
  static void TearDownTestSuite() {
    DeleteEntry(file_handler_->GetParentPath());
  }
 protected:
  static inline std::unique_ptr<FileHandler> file_handler_= nullptr;
};

TEST_F(FileHandlerTests, FileHandlerCreatesDirectory)
{
  ASSERT_TRUE(DirectoryExists(file_handler_->GetParentPath()));
}


TEST_F(FileHandlerTests, FileHandlerFOpenClose)
{
  auto file= AppendPath(file_handler_->GetParentPath()/"file0.txt");
  file_handler_->Upsert(file.filename());

  EXPECT_FALSE(file_handler_->Open(file.filename()));
  EXPECT_TRUE(file_handler_->Close(file.filename()));
}

TEST_F(FileHandlerTests, FileHandlerFCloseOpen)
{
  auto file= AppendPath(file_handler_->GetParentPath()/"file_01.txt");
  file_handler_->Upsert(file.filename());
  EXPECT_TRUE(file_handler_->Close(file.filename()));
  EXPECT_TRUE(file_handler_->Open(file.filename()));
}

TEST_F(FileHandlerTests, FileHandlerFUpsert)
{
  auto file= AppendPath(file_handler_->GetParentPath()/"file.txt");
  file_handler_->Upsert(file.filename());

  EXPECT_TRUE(file_handler_->Contains(file));
  EXPECT_TRUE(FileExists(file));
}


TEST_F(FileHandlerTests, FileHandlerFRepeatedUpsert)
{
  auto file= AppendPath(file_handler_->GetParentPath()/"file2.txt");
  file_handler_->Upsert(file.filename());

  EXPECT_TRUE(file_handler_->Upsert(file.filename()));
  EXPECT_TRUE(FileExists(file));
}

TEST_F(FileHandlerTests, FileHandlerFUpsertReOpensFile)
{
  auto file= AppendPath(file_handler_->GetParentPath()/"file3.txt");
  file_handler_->Upsert(file.filename());

  file_handler_->Close(file.filename());

  EXPECT_TRUE(file_handler_->Upsert(file.filename()));
}

TEST_F(FileHandlerTests, FileHandlerFNotExist)
{
  auto file= AppendPath(file_handler_->GetParentPath()/"file4.txt");
  EXPECT_FALSE(file_handler_->Contains(file.filename()));
  EXPECT_FALSE(file_handler_->Delete(file.filename()));
}
TEST_F(FileHandlerTests, FileHandlerFDelete)
{
  auto file= AppendPath(file_handler_->GetParentPath()/"file5.txt");
  file_handler_->Upsert(file.filename());
  file_handler_->Delete(file.filename());
  EXPECT_FALSE(file_handler_->Contains(file.filename()));
}
