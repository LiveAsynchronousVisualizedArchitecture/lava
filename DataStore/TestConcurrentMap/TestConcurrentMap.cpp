// TestConcurrentMap.cpp : Defines the entry point for the console application.
//
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char* const argv[]) {
    int result = Catch::Session().run(argc, argv);

    std::cin.get();
    return result;
}