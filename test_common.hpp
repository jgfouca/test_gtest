#pragma once

#include <gtest/gtest.h>
#include <random>
#include <vector>
#include <string>
#include <memory>

namespace test_common {

namespace utils {

inline int getTestSeed() {
  static int seed = []() -> int {
    int s = testing::GTEST_FLAG(random_seed);
    if (s == 0) {
      s = std::chrono::high_resolution_clock::now().time_since_epoch().count() % INT32_MAX;
    }
    return s;
  }();

  return seed;
}

inline int& randSeedState() {
  static int s = 0;
  return s;
}

inline void initRandSeed() {
  int seed = getTestSeed();
  std::srand(static_cast<unsigned int>(seed));
  randSeedState() = seed;
}

}

class MyTrackingFixture : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    // Runs once before all tests in this fixture suite
  }

  void SetUp() override {
    // 1. Generate random integer dataset
    utils::initRandSeed();

    // 2. Format a string describing our dataset state
    std::string msg = "rand seed: " + std::to_string(utils::getTestSeed());

    // 3. Keep trace alive for the whole test scope via modern pointer mechanics
    current_trace = std::make_unique<::testing::ScopedTrace>(__FILE__, __LINE__, msg);
  }

  void TearDown() override {
    current_trace.reset();
  }

  std::unique_ptr<::testing::ScopedTrace> current_trace;
};

}
