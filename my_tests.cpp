#include <gtest/gtest.h>
#include <random>
#include <vector>
#include <string>
#include <memory>

class MyTrackingFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // Runs once before all tests in this fixture suite
    }

    void SetUp() override {
        // 1. Generate random integer dataset
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(1, 100);

        random_data.clear();
        for (int i = 0; i < 5; ++i) {
            random_data.push_back(distr(gen));
        }

        // 2. Format a string describing our dataset state
        std::string msg = "Random seed dataset for this test run: [";
        for (size_t i = 0; i < random_data.size(); ++i) {
            msg += std::to_string(random_data[i]) + (i == random_data.size() - 1 ? "" : ", ");
        }
        msg += "]";

        // 3. Keep trace alive for the whole test scope via modern pointer mechanics
        current_trace = std::make_unique<::testing::ScopedTrace>(
            __FILE__, __LINE__, msg
        );
    }

    void TearDown() override {
        current_trace.reset();
    }

    std::vector<int> random_data;
    std::unique_ptr<::testing::ScopedTrace> current_trace;
};

// This test WILL FAIL intentionally so you can visually see the dynamic trace output!
TEST_F(MyTrackingFixture, IntentionalFailureDemo) {
    ASSERT_EQ(random_data.size(), 5);

    // Triggering an intentional failure to show the generated trace stream
    EXPECT_TRUE(false) << "We failed this intentionally to print the active scoped trace below!";
}

// This test will pass successfully
TEST_F(MyTrackingFixture, PassingDemo) {
    EXPECT_EQ(random_data.size(), 5);
    for (int val : random_data) {
        EXPECT_GE(val, 1);
        EXPECT_LE(val, 100);
    }
}
