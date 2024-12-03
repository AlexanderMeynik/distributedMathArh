
#pragma once
#ifndef DIPLOM_FILEHANDLER_H
#define DIPLOM_FILEHANDLER_H

#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <memory>

namespace fileUtils {
    namespace fs = std::filesystem;
    fs::path getNormalAbs(const fs::path &path);

    using osPtr = std::shared_ptr<std::ofstream>;

    class fileHandler {
    public:
        explicit fileHandler(const std::string &parent_path);

        void open(const std::string &filename, std::ios_base::openmode mode = std::ios_base::out);

        void close(const std::string &filename);

        bool upsert(const std::string &filename, std::ios_base::openmode mode = std::ios_base::out);

        void closeFiles();

        ~fileHandler() {
            closeFiles();
        }

        template<typename T>
        void print(osPtr &outPtr, const T &pr);

        template<bool checks = true, typename T>
        void output(const std::string &filename, const T &pr);
        const fs::path &getParentPath() const;
    private:
        fs::path parentPath;
        std::unordered_map<std::string, osPtr> fileMap;
    };



    template<typename T>
    void fileHandler::print(osPtr &outPtr, const T &pr) {
       /* std::ostream &out = *outPtr;*/
        *outPtr << pr;
    }


    template<bool checks, typename T>
    void fileHandler::output(const std::string &filename, const T &pr) {
        auto filePath = parentPath / filename;
        if constexpr (checks) {
            if (fileMap.contains(filePath) || fileMap[filePath]->is_open()) {
                if (bool res = upsert(filename);!res) {
                    return;
                }

            }
        }
        print(fileMap[filePath], pr);
    }
}
#endif //DIPLOM_FILEHANDLER_H
