#include <chrono>
#include <thread>
#include <vector>

#include "catch.hpp"
#include "ConcurrentMap.hpp"

TEST_CASE("Test ConcurrentHash.size()", "[ConcurrentHash]") {
    ConcurrentHash ch(8);
    REQUIRE(ch.size() == 8);
}

TEST_CASE("Test ConcurrentHash.put()", "[ConcurrentHash]") {
    ConcurrentHash ch(8);
    for (uint32_t i = 0; i < ch.size(); i++) {
        ch.put(i, i * i);
    }
    for (uint32_t i = 0; i < ch.size(); i++) {
        REQUIRE(ch.get(i) == i * i);
    }
}

TEST_CASE("Test ConcurrentHash.get()", "[ConcurrentHash]") {
    ConcurrentHash ch(8);

    SECTION("Getting non-existent key returns EMPTY_KEY") {
        uint32_t result = ch.get(100);
        REQUIRE(result == ConcurrentHash::EMPTY_KEY);
    }
}

TEST_CASE("Test ConcurrentHash.init()", "[ConcurrentHash]") {

    SECTION("Size is rounded up to next power of 2") {
        ConcurrentHash ch1(7);
        REQUIRE(ch1.size() == 8);

        ConcurrentHash ch2(9);
        REQUIRE(ch2.size() == 16);
    }
}

TEST_CASE("Test for race conditions while using multiple threads", "[ConcurrentHash][threads]") {
    ConcurrentHash ch(64);
    std::vector<std::thread> thrds;
    int numThreads = 24;

    SECTION("Threads don't interfere with each other, with join") {
        for (int i = 0; i < numThreads; i++) {
            thrds.push_back(std::thread([&ch]() {
                for (uint32_t j = 0; j < ch.size(); j++) {
                    ch.put(j, j * j);
                    REQUIRE(ch.get(j) == j * j);
                }
            }));
        }

        for (int i = 0; i < numThreads; i++) {
            thrds[i].join();
        }
    }

    SECTION("Threads don't interfere with each other, with detach") {
        for (int i = 0; i < numThreads; i++) {
            std::thread([&ch]() {
                for (uint32_t j = 0; j < ch.size(); j++) {
                    ch.put(j, j * j);
                    REQUIRE(ch.get(j) == j * j);
                }
            }).detach();
        }
    }

    // Give detached threads enough time to finish since they cannot access
    // the ConcurrentHash instance after this test case goes out of scope
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}