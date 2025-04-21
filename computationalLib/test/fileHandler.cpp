#include "fileHandler.h"

/// file_utils namespace
namespace file_utils {

fs::path GetNormalAbs(const fs::path &path) {
  return fs::absolute(path).lexically_normal();
}

bool CreateDirIfNotPresent(const std::string &path) {
  if (!fs::exists(path)) {
    return fs::create_directories(path);
  }
  return fs::is_directory(path);
}

fileHandler::fileHandler(const std::string &parent_path) :
    parentPath(parent_path) {
  CreateDirIfNotPresent(parentPath);
}

void fileHandler::Open(const std::string &filename, std::ios_base::openmode mode) {
  auto file_path = parentPath / filename;
  fileMap[file_path]->open(file_path, mode);
}

void fileHandler::Close(const std::string &filename) {
  auto file_path = parentPath / filename;
  fileMap[file_path]->close();
}

bool fileHandler::Upsert(const std::string &filename, std::ios_base::openmode mode) {
  auto file_path = parentPath / filename;
  if (!fileMap.contains(file_path)) {
    fileMap[file_path] = std::make_shared<std::ofstream>();
  }
  if (!fileMap[file_path]->is_open()) {
    fileMap[file_path]->open(file_path, mode);
  }
  return fileMap[file_path]->is_open();

}

void fileHandler::CloseFiles() {
  for (auto &ptr : fileMap) {
    ptr.second->close();
  }
}

const fs::path &fileHandler::GetParentPath() const {
  return parentPath;
}

bool fileHandler::Contains(const std::string &filename) {
  return fileMap.contains(parentPath / filename);
}

}
