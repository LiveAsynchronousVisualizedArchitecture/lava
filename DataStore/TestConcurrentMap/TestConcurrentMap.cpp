// TestConcurrentMap.cpp: Main entry point for unit tests covering the ConcurrentMap project

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char* const argv[]) {
    int result = Catch::Session().run(argc, argv);
    std::cin.get();
    return result;
}