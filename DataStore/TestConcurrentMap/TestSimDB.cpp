// Tests for the SimDB class

#include <string>
#include <vector>

#include "catch.hpp"
#include "ConcurrentMap.hpp"


TEST_CASE("SimDB.get(const char *key)", "[simdb]") {

  SECTION("Can call get with c_str as key", "[simdb]") {
    simdb db("test", 16, 16);
    char *key = "my_key";
    char *val = "my_val";
    char result[6];
    db.put((void*)key, (ui32)strlen(key), (void*)val, (ui32)strlen(val));
    i64 len = db.get(key, result);

    REQUIRE(len == 6);
    REQUIRE(strcmp(result, val));
  }

  SECTION("Can call get with vector as key", "[simdb]") {
    simdb db("test", 16, 16);
    std::vector<ui32> my_vec = { 1, 2, 3 };
    char *val = "my_val";
    char result[6];

    i32 kidx = db.put((void*)my_vec.data(), (ui32)(my_vec.size() * sizeof(ui32)), (void*)val, (ui32)strlen(val));

    i64 len = db.get<ui32>(my_vec, result);

    REQUIRE(len == 6);
    REQUIRE(strcmp(result, val));
  }
}
