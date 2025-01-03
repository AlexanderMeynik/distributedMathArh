#pragma once
#ifndef DATA_DEDUPLICATION_SERVICE_MYCONCEPTS_H
#define DATA_DEDUPLICATION_SERVICE_MYCONCEPTS_H

#include "clockArray.h"//todo only needed for clock(what's puprose of this file)

/// commonDeclarations namespace
namespace commonDeclarations {
    /**
    * Global clock used for time measurement
    */
    using clockType = timing::chronoClockTemplate<std::milli>;
    using symbolType = char;
    extern clockType gClk;


    /**
     * Prinatble concept
     * @tparam T
     */
    template<typename T>
    concept printable = requires(const T &elem, std::ofstream &out){
        { out << elem } -> std::same_as<std::ostream &>;
    };




    /**
     * Print format function
     * @param zcFormat
     * @param ...
     */
    std::string vformat(const char *zcFormat, ...);

    /**
     * Return codes enum
     */
    enum returnCodes {
        WarningMessage = -3,
        AlreadyExists = -2,
        ErrorOccured = -1,
        ReturnSucess = 0
    };

    /**
     *  Parameter type enum
     */
    enum paramType {
        EmptyParameterValue = -1
    };


} // namespace commonDeclarations

#endif  // DATA_DEDUPLICATION_SERVICE_MYCONCEPTS_H
