#include <random>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <regex>

#include <gtest/gtest.h>
#include <Kokkos_Core.hpp>
#include <Kokkos_Random.hpp>
#include <KokkosSparse_IOUtils.hpp>
#include <KokkosKernels_TestUtils.hpp>

#include "test_common.hpp"

using test_common::MyTrackingFixture;

using device_t    = Kokkos::Device<Kokkos::DefaultExecutionSpace, typename Kokkos::DefaultExecutionSpace::memory_space>;
using view_1di_t  = Kokkos::View<int*, device_t>;
using view_1dd_t  = Kokkos::View<double*, device_t>;
using sp_matrix_t = KokkosSparse::CrsMatrix<double, int, device_t, void, int>;

// specs for sparse mtxs
static constexpr int N = 100;
static constexpr int D = 1;
static constexpr int NNZ = 5*N;
static constexpr int B = 20;

// Convert to a single view for easier baseline processing
auto condense(const sp_matrix_t& A)
{
  // Pull out views from CRS
  auto row_map = A.graph.row_map;
  auto entries = A.graph.entries;
  auto values  = A.values;
  const auto total = row_map.size() + entries.size() + values.size();
  view_1dd_t v("vcondensed", total);

  Kokkos::parallel_for(total, KOKKOS_LAMBDA(const size_t i) {
    if (i < row_map.size()) {
      v(i) = static_cast<double>(row_map(i));
    }
    else if (i < (row_map.size() + entries.size())) {
      v(i) = static_cast<double>(entries(i - row_map.size()));
    }
    else {
      v(i) = values(i - (row_map.size() + entries.size()));
    }
  });

  auto hv = Kokkos::create_mirror_view(v);
  Kokkos::deep_copy(hv, v);

  return hv;
}

template <typename T>
void process_baseline(const T* data, size_t size) {
  const ::testing::UnitTest* const unit_test = ::testing::UnitTest::GetInstance();
  const ::testing::TestInfo* const test_info = unit_test->current_test_info();

  if (!test_info || !data || size == 0) {
    ASSERT_TRUE(false) << "Not in a test?";
  }

  // 1. Get the original test name
  std::string test_name = test_info->name();

  // 2. Strip "_N" suffix if it exists at the end of the test name
  // Matches an underscore followed by one or more digits at the end of the string ($)
  std::regex suffix_regex(R"(_\d+$)");
  test_name = std::regex_replace(test_name, suffix_regex, "");

  // Construct file name using suite, name, and current random seed
  const auto seed = test_common::utils::getTestSeed();
  std::string file_name = std::string(test_info->test_suite_name()) + "_" +
    test_name + "_seed_" +
    std::to_string(seed) + ".bin"; // Binary recommended for raw memory

  std::filesystem::path full_path = std::filesystem::path("baselines") / file_name;
  const size_t byte_size = size * sizeof(T);

  // Write Mode: If file does not exist, save the baseline data
  if (!std::filesystem::exists(full_path)) {
    std::ofstream out(full_path, std::ios::binary);
    ASSERT_TRUE(out) << "Could not open output file: " << full_path;

    out.write(reinterpret_cast<const char*>(data), byte_size);
    std::cout << "Writing baseline " << full_path << std::endl;
  }
  else {
    // Read Mode: If file exists, verify the contents
    std::ifstream in(full_path, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(in) << "Could not open input file: " << full_path;

    // File size check to prevent buffer overruns
    ASSERT_TRUE(static_cast<size_t>(in.tellg()) == byte_size) << "Data size did not match! " << in.tellg() << " != " << byte_size;
    in.seekg(0, std::ios::beg);
    std::cout << "Reading baseline " << full_path << std::endl;

    // Read and compare elements directly in memory
    std::vector<T> baseline_data(size);
    in.read(reinterpret_cast<char*>(baseline_data.data()), byte_size);

    EXPECT_TRUE(std::equal(data, data + size, baseline_data.begin())) << "Data did not match!";
  }
}

inline void report_random_num()
{
  const auto r = rand();
  process_baseline(&r, 1);
}

inline void report_mt19937_num()
{
  std::mt19937 gen{test_common::utils::getTestSeed()};
  std::uniform_real_distribution<double> values_distribution(-10, 10);
  double randv = values_distribution(gen);
  process_baseline(&randv, 1);
}

inline void report_fill_random()
{
  Kokkos::Random_XorShift64_Pool<Kokkos::DefaultExecutionSpace> pool(test_common::utils::getTestSeed());
  view_1di_t v("v", 10);
  Kokkos::fill_random(v, pool, 0, 100);
  auto hv = Kokkos::create_mirror_view(v);
  Kokkos::deep_copy(hv, v);
  process_baseline(hv.data(), hv.size());
}

inline void report_kk_random_sparse_matrix()
{
  int nnz = NNZ; // needs to be non const
  auto A = KokkosSparse::Impl::kk_generate_diagonally_dominant_sparse_matrix<sp_matrix_t>(N, N, nnz, 0, B, D);
  auto Ac = condense(A);
  process_baseline(Ac.data(), Ac.size());
}

inline void report_kk_rand_cs_matrix()
{
  TestUtils::RandCsMatrix<double, Kokkos::LayoutRight, device_t, int, int> csMat(N, N, 0, 10, false);
  sp_matrix_t mtx("crs", N, N, csMat.get_vals().size(), csMat.get_vals(), csMat.get_map(), csMat.get_ids());
  auto mtxc = condense(mtx);
  process_baseline(mtxc.data(), mtxc.size());
}

inline void report_kk_random_matrix()
{
  auto A = TestUtils::randomMatrix<sp_matrix_t, int>(N, N, NNZ-10, NNZ+10, false);
  auto Ac = condense(A);
  process_baseline(Ac.data(), Ac.size());
}

// This test WILL FAIL intentionally so you can visually see the dynamic trace output!
TEST_F(MyTrackingFixture, IntentionalFailureDemo) {
  // Triggering an intentional failure to show the generated trace stream
  EXPECT_TRUE(false) << "We failed this intentionally to print the active scoped trace below!";
}

// This test WILL FAIL intentionally so you can visually see the dynamic trace output!
TEST_F(MyTrackingFixture, IntentionalFailureExcDemo) {
  // Triggering an intentional failure to show the generated trace stream
  throw std::runtime_error("error");
}

// This test WILL FAIL intentionally so you can visually see the dynamic trace output!
// The seg fault blocks running of subsequent tests, so we comment this out.
// Segfaults terminate the process instantly, so the seed is NOT reported.
// TEST_F(MyTrackingFixture, IntentionalFailureSegDemo) {
//   // Triggering an intentional failure to show the generated trace stream
//   std::vector<double> foo;
//   std::cout << foo[12345] << std::endl;
// }

TEST_F(MyTrackingFixture, rand_1) {
  report_random_num();
}

TEST_F(MyTrackingFixture, rand_2) {
  report_random_num();
}

TEST_F(MyTrackingFixture, mt19937_1) {
  report_mt19937_num();
}

TEST_F(MyTrackingFixture, mt19937_2) {
  report_mt19937_num();
}

TEST_F(MyTrackingFixture, fill_random_1) {
  report_fill_random();
}

TEST_F(MyTrackingFixture, fill_random_2) {
  report_fill_random();
}

TEST_F(MyTrackingFixture, kk_random_sparse_matrix_1) {
  report_kk_random_sparse_matrix();
}

TEST_F(MyTrackingFixture, kk_random_sparse_matrix_2) {
  report_kk_random_sparse_matrix();
}

TEST_F(MyTrackingFixture, kk_rand_cs_matrix_1) {
  report_kk_rand_cs_matrix();
}

TEST_F(MyTrackingFixture, kk_rand_cs_matrix_2) {
  report_kk_rand_cs_matrix();
}

TEST_F(MyTrackingFixture, kk_random_matrix_1) {
  report_kk_random_matrix();
}

TEST_F(MyTrackingFixture, kk_random_matrix_2) {
  report_kk_random_matrix();
}
