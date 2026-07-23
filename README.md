# Gtest random seed testing

Things to check:
* Upon test fail, the random seed is reported
* The random numbers generated are the same for a test regardless of other tests that might be run
* When a test is repeated with the same seed specified, the same random numbers are generated
* Upon each run without a specified seed, a new seed is used.

Other sources of randomness:
* mt19937
* Kokkos::fill_random
* Kokkos::Random_XorShift64_Pool
* uniform_real_distribution
* random_device
* kk_generate_sparse_matrix (or other similar calls)
* TestUtils::RandCsMatrix
* Be sure to remove all srand
* randomMatrix
* getRandomBounds
* std::shuffle

Pros:
* When compared to permanent fixed seed, coverage is improved since we are testing a new seed every run

Cons:
* Every failing test will report random seed even if it's not a randomized test
* segfaulting tests will not report seed