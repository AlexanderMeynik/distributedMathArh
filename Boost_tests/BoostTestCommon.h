//
// Created by Lenovo on 22.07.2024.
//

#ifndef DIPLOM_BOOSTTESTCOMMON_H
#define DIPLOM_BOOSTTESTCOMMON_H
#define CHECK_CLOSE_COLLECTION(aa, bb, tolerance) { \
    using std::distance; \
    using std::begin; \
    using std::end; \
    auto a = begin(aa), ae = end(aa); \
    auto b = begin(bb); \
    BOOST_REQUIRE_EQUAL(distance(a, ae), distance(b, end(bb))); \
    for(; a != ae; ++a, ++b) { \
        BOOST_CHECK_CLOSE(*a, *b, tolerance); \
    } \
}
#endif //DIPLOM_BOOSTTESTCOMMON_H
