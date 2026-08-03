# Gtest random seed testing

Things to check:
* Upon test fail, the random seed is reported
* The random numbers generated are the same for a test regardless of other tests that might be run
* When a test is repeated with the same seed specified, the same random numbers are generated
* Upon each run without a specified seed, a new seed is used.

How things are checked:
* We have a couple tests that are intentionally failed in order to confirm that the seed is printed
* The other tests write their results to a baseline file
* The other tests are duplicated with a _1 and _2 version that share a baseline
* A full test run confirms that running the same test multiple times with the same seed produces the same result and is not impacted by other test runs
* You can then re-run the specific tests and specific the same seed as before. A baseline should be found and result should match
* You can then re-run the specific tests without providing a seed and a new seed should be used each time.
* You can also diff the baselines with the same test but different seeds to confirm the test is producing the same result regardless of seed

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
* getRandomBounds -- Despite name, this is not random at all
* std::shuffle
* The kokkos random stuff must be tested on some devices

Pros:
* When compared to permanent fixed seed, coverage is improved since we are testing a new seed every run
* When a test fails, it should be easy to repeat exactly what it did via --gtest_random_seed

Cons:
* Every failing test will report random seed even if it's not a randomized test
* segfaulting tests will not report seed

Building:
* For serial CPU, it's trivial.
* For lychee/blake, I did: cmake -DKokkos_ARCH_HOPPER90=ON -DCMAKE_CUDA_ARCHITECTURES=90 -DKokkos_ENABLE_AGGRESSIVE_VECTORIZATION=Off -DKokkos_ENABLE_CUDA=On -DKokkos_ENABLE_CUDA_LAMBDA=On -DCMAKE_CXX_COMPILER=$HOME/kokkos/bin/nvcc_wrapper ..
