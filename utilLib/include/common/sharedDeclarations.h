#ifndef DIPLOM_COMMONCONSTANTS_H
#define DIPLOM_COMMONCONSTANTS_H
/**
 * @brief shared namespace
 * @details This namespace contains multiple forward declarations for types to be use everywhere
 */
//todo forward declare some things
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
}
#endif //DIPLOM_COMMONCONSTANTS_H
