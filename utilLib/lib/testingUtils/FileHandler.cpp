#include "testingUtils/FileHandler.h"

/// file_utils namespace
namespace file_utils {

FsPath GetNormalAbs(const FsPath &path) {
  return fs::absolute(path).lexically_normal();
}

bool CreateDirIfNotPresent(const FsPath &path) {
  if (!fs::exists(path)) {
    return fs::create_directories(path);
  }
  return false;
}

bool CreateFileIfNotPresent(const FsPath &path,bool force) {
  if(auto p =path.parent_path();force && !fs::exists(p))
  {
    fs::create_directories(p);
  }

  if (!fs::exists(path)) {
    return std::ofstream(path).operator bool();
  }
  return false;
}

bool FileExists(const FsPath &path) {
  return fs::exists(path) && fs::is_regular_file(path);
}
bool DirectoryExists(const FsPath &path) {
  return fs::exists(path) && fs::is_directory(path);
}
std::string ReadFileToString(const FsPath &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + path.string());
  }
  return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}
uintmax_t DeleteEntry(const FsPath &path) {
  std::error_code eer;
  bool del=fs::remove(path,eer);
  if(!del&&fs::is_directory(path))
  {
    return fs::remove_all(path);
  }
  return del;
}


FileHandler::FileHandler(const std::string &parent_path) :
    parent_path_(parent_path) {
  CreateDirIfNotPresent(parent_path_);
}

void FileHandler::Open(const std::string &filename, std::ios_base::openmode mode) {
  auto file_path = parent_path_ / filename;
  file_map_[file_path]->open(file_path, mode);
}

void FileHandler::Close(const std::string &filename) {
  auto file_path = parent_path_ / filename;
  file_map_[file_path]->close();
}

bool FileHandler::Upsert(const std::string &filename, std::ios_base::openmode mode) {
  auto file_path = parent_path_ / filename;
  if (!file_map_.contains(file_path)) {
    file_map_[file_path] = std::make_shared<std::ofstream>();
  }
  if (!file_map_[file_path]->is_open()) {
    file_map_[file_path]->open(file_path, mode);
  }
  return file_map_[file_path]->is_open();

}

void FileHandler::CloseFiles() {
  for (auto &ptr : file_map_) {
    ptr.second->close();
  }
}

const FsPath &FileHandler::GetParentPath() const {
  return parent_path_;
}

bool FileHandler::Contains(const std::string &filename) {
  return file_map_.contains(parent_path_ / filename);
}

}
