#include <iostream>
#include <gtest/gtest.h>
#include <random>

#include "test_common.hpp"

using test_common::MyTrackingFixture;

inline void report_random_num()
{
  std::cout << "With seed: " << test_common::utils::getTestSeed() << ", generating random number: " << rand() << std::endl;
}

inline void report_mt19937_num()
{
  std::mt19937 gen{test_common::utils::getTestSeed()};
  std::uniform_real_distribution<double> values_distribution(-10, 10);
  double randv = values_distribution(gen);
  std::cout << "With seed: " << test_common::utils::getTestSeed() << ", generating random double: " << randv << std::endl;
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

// This test WILL FAIL intentionally so you can visually see the dynamic trace output!
TEST_F(MyTrackingFixture, IntentionalFailureExcDemo) {
  report_random_num();
  // Triggering an intentional failure to show the generated trace stream
  throw std::runtime_error("error");
}

// This test WILL FAIL intentionally so you can visually see the dynamic trace output!
// The seg fault blocks running of subsequent tests, so we comment this out.
// Segfaults terminate the process instantly, so the seed is NOT reported.
// TEST_F(MyTrackingFixture, IntentionalFailureSegDemo) {
//   report_random_num();
//   // Triggering an intentional failure to show the generated trace stream
//   std::vector<double> foo;
//   std::cout << foo[12345] << std::endl;
// }

TEST_F(MyTrackingFixture, mt19937_1) {
  report_mt19937_num();
  EXPECT_TRUE(true) << "Should never see this!";
}

TEST_F(MyTrackingFixture, mt19937_2) {
  report_mt19937_num();
  EXPECT_TRUE(true) << "Should never see this!";
}


