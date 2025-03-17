#include "fileHandler.h"
/// fileUtils namespace
namespace fileUtils {

    fs::path getNormalAbs(const fs::path &path) {
        return fs::absolute(path).lexically_normal();
    }

    bool createDirIfNotPresent(const std::string &path) {
        if (!fs::exists(path)) {
            return fs::create_directories(path);
        }
        return fs::is_directory(path);
    }


    fileHandler::fileHandler(const std::string &parent_path) :
            parentPath(parent_path) {
        createDirIfNotPresent(parentPath);
    }

    void fileHandler::open(const std::string &filename, std::ios_base::openmode mode) {
        auto filePath = parentPath / filename;
        fileMap[filePath]->open(filePath, mode);
    }

    void fileHandler::close(const std::string &filename) {
        auto filePath = parentPath / filename;
        fileMap[filePath]->close();
    }

    bool fileHandler::upsert(const std::string &filename, std::ios_base::openmode mode) {
        auto filePath = parentPath / filename;
        if (!fileMap.contains(filePath)) {
            fileMap[filePath] = std::make_shared<std::ofstream>();
        }
        if (!fileMap[filePath]->is_open()) {
            fileMap[filePath]->open(filePath, mode);
        }
        return fileMap[filePath]->is_open();

    }


    void fileHandler::closeFiles() {
        for (auto &ptr: fileMap) {
            ptr.second->close();
        }
    }

    const fs::path &fileHandler::getParentPath() const {
        return parentPath;
    }

    bool fileHandler::contains(const std::string &filename) {
        return fileMap.contains(parentPath / filename);
    }


}
