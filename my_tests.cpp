#include <random>
#include <iostream>

#include <gtest/gtest.h>
#include <Kokkos_Core.hpp>
#include <Kokkos_Random.hpp>
#include <KokkosSparse_IOUtils.hpp>

#include "test_common.hpp"

using test_common::MyTrackingFixture;

using device_t    = Kokkos::Device<Kokkos::DefaultExecutionSpace, typename Kokkos::DefaultExecutionSpace::memory_space>;
using view_1di_t  = Kokkos::View<int*, device_t>;
using sp_matrix_t = KokkosSparse::CrsMatrix<double, int, device_t, void, size_t>;

// specs for sparse mtxs
static constexpr auto N = 100;
static constexpr auto D = 1;


template <typename view_t>
typename view_t::non_const_value_type sum(const view_t& view)
{
  using value_t = typename view_t::non_const_value_type;
  value_t the_sum = 0;
  Kokkos::parallel_reduce(view.size(), KOKKOS_LAMBDA(const size_t i, value_t& inner) {
    inner += view(i);
  }, the_sum);
  return the_sum;
}

// Poor man's csr hash
double csr_sum(const sp_matrix_t& A)
{
  // Pull out views from CRS
  auto row_map = A.graph.row_map;
  auto entries = A.graph.entries;
  auto values  = A.values;

  return sum(row_map) + sum(entries) + sum(values);
}

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

inline void report_fill_random()
{
  Kokkos::Random_XorShift64_Pool<Kokkos::DefaultExecutionSpace> pool(test_common::utils::getTestSeed());
  view_1di_t v("v", 10);
  Kokkos::fill_random(v, pool, 0, 100);
  std::cout << "With seed: " << test_common::utils::getTestSeed() << ", fill sum: " << sum(v) << std::endl;
}

inline void report_kk_random_sparse_matrix()
{
  size_t nnz = 5 * N;
  auto A        = KokkosSparse::Impl::kk_generate_diagonally_dominant_sparse_matrix<sp_matrix_t>(
      N, N, nnz, 0, 20, D);
  std::cout << "With seed: " << test_common::utils::getTestSeed() << ", csr sum: " << csr_sum(A) << std::endl;
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

TEST_F(MyTrackingFixture, fill_random_1) {
  report_fill_random();
  EXPECT_TRUE(true) << "Should never see this!";
}

TEST_F(MyTrackingFixture, fill_random_2) {
  report_fill_random();
  EXPECT_TRUE(true) << "Should never see this!";
}

TEST_F(MyTrackingFixture, kk_random_sparse_matrix_1) {
  report_kk_random_sparse_matrix();
  EXPECT_TRUE(true) << "Should never see this!";
}

TEST_F(MyTrackingFixture, kk_random_sparse_matrix_2) {
  report_kk_random_sparse_matrix();
  EXPECT_TRUE(true) << "Should never see this!";
}
