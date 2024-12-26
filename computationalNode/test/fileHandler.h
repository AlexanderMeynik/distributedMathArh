
#pragma once
#ifndef DIPLOM_FILEHANDLER_H
#define DIPLOM_FILEHANDLER_H

#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <memory>

/// fileUtils namespace
namespace fileUtils {
    namespace fs = std::filesystem;
    fs::path getNormalAbs(const fs::path &path);

    bool createDirIfNotPresent(const std::string&path);

    using osPtr = std::shared_ptr<std::ofstream>;

    /**
     * @brief Sample class that handles dynmic file opening and output
     */
    class fileHandler {
    public:
        /**
         * @details Creates parent_path if it doesn't exist;
         * @details filenames in this class are stored in format parent_path/filename
         * @param parent_path
         */
        explicit fileHandler(const std::string &parent_path);

        /**
         * @brief Opens file with given permissions
         * @details filename must be present in class
         * @param filename
         */
        void open(const std::string &filename, std::ios_base::openmode mode = std::ios_base::out);

        /**
         * @brief Closes filename with given name
         * @param filename
         */
        void close(const std::string &filename);

        /**
         * @brief Adds and opens filename to write with given permissions
         * @details If file is already present in fileHandler it'l be opened in case it's closed
         * @param filename
         * @param mode
         * @return file->is_open()
         */
        bool upsert(const std::string &filename, std::ios_base::openmode mode = std::ios_base::out);

        /**
         * @brief Contains wrapper
         * @param filename
         */
        bool contains(const std::string &filename);

        /**
         * @brief Closes all files from this instance of fileHandler
         */
        void closeFiles();

        ~fileHandler() {
            closeFiles();
        }

        /**
         * @brief Print pr to selected outPtr
         * @details user mus implement [std::ostream& operator<<](https://en.cppreference.com/w/cpp/io/basic_ostream/operator_ltlt) for it
         * @tparam T
         * @param outPtr
         * @param pr
         */
        template<typename T>
        void print(osPtr &outPtr, const T &pr);

        /**
         * @brief Wrapper for fileHandler::print to print data to designated file
         * @tparam checks
         * @tparam T
         * @param filename
         * @see fileHandler::print
         */
        template<bool checks = true, typename T>
        void output(const std::string &filename, const T &pr);
        /**
         *
         */
        const fs::path &getParentPath() const;
    private:
        fs::path parentPath;
        std::unordered_map<std::string, osPtr> fileMap;
    };



    template<typename T>
    void fileHandler::print(osPtr &outPtr, const T &pr) {
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