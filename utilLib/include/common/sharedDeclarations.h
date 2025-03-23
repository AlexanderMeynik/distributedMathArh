#pragma once
#ifndef DIPLOM_SHARED_DECLARATIONS
#define DIPLOM_SHARED_DECLARATIONS

#include <iosfwd>
#include <string>
#include <unordered_map>
/**
 * @brief shared namespace
 * @details This namespace contains multiple forward declarations for types to be use everywhere
 */
namespace shared {
    using FloatType = double;
    /**
     * @brief Common numeric parameters values
     */
    struct params {
        static constexpr FloatType c = 3.0 * 1e8;
        static constexpr FloatType yo = 1e7;
        static constexpr FloatType omega = 1e15;
        static constexpr FloatType omega0 = omega;
        static constexpr FloatType a = 1;
        static constexpr FloatType eps = 1;
    };


    enum class state_t {
        new_,
        old,
        openmp_new,
        openmp_old,
        print_
    };

    constexpr std::array<const char*,5> stateToStr=
            {
                    "openmp_new",
                    "new",
                    "openmp_old",
                    "old",
                    "printImpl"
            };

    const static std::unordered_map<std::string, state_t> stringToState = {
            {"openmp_new", state_t::openmp_new},
            {"new",        state_t::new_},
            {"openmp_old", state_t::openmp_old},
            {"old",        state_t::old},
            {"printImpl",  state_t::print_},
    };

    std::ostream &operator<<(std::ostream &out, const state_t &st);


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

}
#endif //DIPLOM_SHARED_DECLARATIONS
