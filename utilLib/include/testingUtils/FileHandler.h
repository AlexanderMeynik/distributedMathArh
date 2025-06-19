#pragma once

#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <memory>

/// fileUtils namespace
namespace file_utils {

namespace fs = std::filesystem;
/// Filesystem path alias
using FsPath=fs::path;
/// Alias for std::ostream ptr
using OstreamPtr = std::shared_ptr<std::ofstream>;

/**
 * @brief Shorthand for absolute lexically normal path
 * @param path
 * @return
 */
FsPath GetNormalAbs(const FsPath &path);

/**
 * @brief Idempotent operation for directory creation
 * @param path
 * @return was directory created
 */
bool CreateDirIfNotPresent(const FsPath &path);

/**
 * @brief Idempotent operation for file creation
 * @param path
 * @param force - creates parent directory if not exist
 * @return was file created
 */
bool CreateFileIfNotPresent(const FsPath &path,bool force = false);

/**
 * @brief Does file exist
 * @param path
 * @return
 */
bool FileExists(const FsPath& path);

/**
 * @brief Does directory exist
 * @param path
 * @return
 */
bool DirectoryExists(const FsPath& path);

/**
 * @brief Removes any entry from filesystem
 * @param path
 * @return number of deleted entries - if path is non empty directory
 * @return 0 - if entry doesn't exist, 1 - if it exists and was deleted
 */
uintmax_t DeleteEntry(const FsPath& path);


/**
 * @brief Retrieves file contents in form of string
 * @param path
 * @return files contents
 * @throws runtime_error - file cannot be opened
 */
std::string ReadFileToString(const FsPath& path);

/**
 * @brief Sample class that handles dynamic file opening and Output
 */
class FileHandler {
 public:
  /**
   * @details Creates parent_path if it doesn't exist;
   * @details filenames in this class are stored in format parent_path/filename
   * @param parent_path
   */
  explicit FileHandler(const std::string &parent_path);

  /**
   * @brief Opens file with given permissions
   * @details filename must be present in class
   * @param filename
   */
  void Open(const std::string &filename, std::ios_base::openmode mode = std::ios_base::out);

  /**
   * @brief Closes filename with given name
   * @param filename
   */
  void Close(const std::string &filename);

  /**
   * @brief Adds and opens filename to write with given permissions
   * @details If file is already present in fileHandler it'l be opened in case it's closed
   * @param filename
   * @param mode
   * @return file->is_open()
   */
  bool Upsert(const std::string &filename, std::ios_base::openmode mode = std::ios_base::out);

  /**
   * @brief Contains wrapper
   * @param filename
   */
  bool Contains(const std::string &filename);

  /**
   * @brief Closes all files from this instance of fileHandler
   */
  void CloseFiles();

  ~FileHandler() {
    CloseFiles();
  }

  /**
   * @brief Print pr to selected outPtr
   * @details user mus implement [std::ostream& operator<<](https://en.cppreference.com/w/cpp/io/basic_ostream/operator_ltlt) for it
   * @tparam T
   * @param out_ptr
   * @param pr
   */
  template<typename T>
  void Print(OstreamPtr &out_ptr, const T &pr);

  /**
   * @brief Wrapper for fileHandler::Print to printImpl data to designated file
   * @tparam checks
   * @tparam T
   * @param filename
   * @param printed_val
   * @see fileHandler::printImpl
   */
  template<bool checks = true, typename T>
  void Output(const std::string &filename, const T &printed_val);

  /**
   * @brief Getter for parent dir path
   */
  const FsPath &GetParentPath() const;

 private:
  FsPath parent_path_;
  std::unordered_map<std::string, OstreamPtr> file_map_;
};

template<typename T>
void FileHandler::Print(OstreamPtr &out_ptr, const T &pr) {
  *out_ptr << pr;
}

template<bool checks, typename T>
void FileHandler::Output(const std::string &filename, const T &printed_val) {
  auto file_path = parent_path_ / filename;
  if constexpr (checks) {
    if (file_map_.contains(file_path) || file_map_[file_path]->is_open()) {
      if (bool res = Upsert(filename);!res) {
        return;
      }
    }
  }
  Print(file_map_[file_path], printed_val);
}
}
