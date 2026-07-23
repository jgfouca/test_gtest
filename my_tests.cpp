#include <iostream>
#include <gtest/gtest.h>
#include <random>

#include "test_common.hpp"

using test_common::MyTrackingFixture;

inline void report_random_num()
{
  std::cout << "With seed: " << test_common::utils::getTestSeed() << ", generating random number: " << rand() << std::endl;
}

// This test WILL FAIL intentionally so you can visually see the dynamic trace output!
TEST_F(MyTrackingFixture, IntentionalFailureDemo) {
  report_random_num();
  // Triggering an intentional failure to show the generated trace stream
  EXPECT_TRUE(false) << "We failed this intentionally to print the active scoped trace below!";
}

// This test will pass successfully
TEST_F(MyTrackingFixture, PassingDemo) {
  report_random_num();
  EXPECT_TRUE(true) << "Should never see this!";
}
