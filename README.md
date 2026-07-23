# Gtest random seed testing

Things to check:
* Upon test fail, the random seed is reported
* The random numbers generated are the same for a test regardless of other tests that might be run
* When a test is repeated with the same seed specified, the same random numbers are generated
* Upon each run without a specified seed, a new seed is used.