// Tests for the SimDB class

#include "catch.hpp"
#include "ConcurrentMap.hpp"


TEST_CASE("SimDB.get(const char *key)", "[SimDB]") {
    simdb db("test", 16, 16);
    char *key = "my_key";
    char *val = "my_val";
    char result[6];
    db.put((void*)key, (ui32)strlen(key), (void*)val, (ui32)strlen(val));
    int len = db.get(key, result);

    REQUIRE(len == 6);
    REQUIRE(strcmp(result, val));
}