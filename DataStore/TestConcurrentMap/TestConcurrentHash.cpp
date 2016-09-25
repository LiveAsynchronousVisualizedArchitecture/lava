#include "catch.hpp"
#include "ConcurrentMap.hpp"

TEST_CASE("test size", "[ConcurrentHash]") {
    ConcurrentHash ch(8);
    REQUIRE(ch.size() == 8);
}

